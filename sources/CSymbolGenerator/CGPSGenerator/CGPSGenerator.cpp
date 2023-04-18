#include "CGPSGenerator.hpp"

#include <cassert>

QCSP::CGPSGenerator::CGPSGenerator(const std::string & tty_gps, bool localtime, bool do_lauch)
    : _tty_path(tty_gps),
      use_localtime(localtime),
      running(false),
      m_frame(),
      counter(0) {
    this->_parser      = new nmea::NMEAParser;
    this->_gps         = new nmea::GPSService(*(this->_parser));
    this->_parser->log = false;

    this->_gps->onLockStateChanged += [](bool is_fixed) {
        std::cout << (is_fixed ? std::string("Fix found!") : std::string("Fix lost...")) << std::endl;
    };

    this->_gps->onUpdate += [this] {
        const float loc_lon  = this->_gps->fix.locked() ? float(this->_gps->fix.longitude) : 0.f;
        const float loc_lat  = this->_gps->fix.locked() ? float(this->_gps->fix.latitude) : 0.f;
        const float loc_time = this->_gps->fix.locked() ? float(this->_gps->fix.timestamp.rawTime) : 0.f;

#if defined(DEBUG) && VERBOSE > 1
        std::cout << (_gps->fix.locked() ? "[*] " : "[ ] ") << loc_lat << " N " << loc_lon << " E" << std::endl;
#endif

        std::unique_lock<std::mutex> lk_frame(this->m_frame);

        this->lon      = loc_lon;
        this->lat      = loc_lat;
        this->raw_time = loc_time;

        lk_frame.unlock();
    };

    this->_tty_gps.open(_tty_path);

    if (do_lauch) {
        this->launch();
    }
}

QCSP::CGPSGenerator::~CGPSGenerator() {
    this->stop();

    // Use pthread API to cancel thread, in case it is block reading the tty.
    pthread_t tid = this->_t->native_handle();
    pthread_cancel(tid);
    pthread_join(tid, nullptr);

    this->_tty_gps.close();
    delete this->_gps;
    delete this->_parser;
}

void QCSP::CGPSGenerator::run() {
    while (this->running.load()) {
        std::string line;
        getline(this->_tty_gps, line);

        try {
            this->_parser->readLine(line);
        } catch (nmea::NMEAParseError & e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

namespace {
struct time_struct {
    uint16_t h_m_s;
    uint8_t  p12_cs;
};

time_struct convert_time(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t cents) {
    const uint8_t hours_12 = hours % 12U;
    const uint8_t plus_12  = hours > 11U ? 0x80U : 0x0U;

    // hhhh mmmmmm ssssss => 4 + 6 + 6 = 16 bits
    const uint16_t h_m_s = ((hours_12 & 0xF) << 12)
                         | ((minutes & 0x3F) << 6)
                         | (seconds & 0x3F);

    // CSymbolGenerator::p cccccccc => 1 + 7 = 8 bits
    const uint8_t p12_cs = plus_12
                         | (cents & 0x7f);

    return {h_m_s, p12_cs};
}

time_struct get_local_time() {
    namespace ch = std::chrono;
    using clk    = ch::system_clock;

    const ch::time_point<clk> now = clk::now();
    const std::time_t         t_c = clk::to_time_t(now);

    constexpr uint8_t tmStrSize = 16;
    char              timeStr[tmStrSize];

    const uint64_t milsecs_now = ch::duration_cast<ch::milliseconds>(now.time_since_epoch()).count();
    const uint64_t seconds_now = ch::duration_cast<ch::seconds>(now.time_since_epoch()).count();
    const uint8_t  cents       = uint8_t((milsecs_now - seconds_now * 1000) / 10);

    strftime(timeStr, tmStrSize, "%H", std::localtime(&t_c));
    const uint8_t hours = std::stoul(timeStr);

    strftime(timeStr, tmStrSize, "%M", std::localtime(&t_c));
    const uint8_t minutes = std::stoul(timeStr);

    strftime(timeStr, tmStrSize, "%S", std::localtime(&t_c));
    const uint8_t seconds = std::stoul(timeStr);

    return convert_time(hours, minutes, seconds, cents);
}

struct time_struct get_gps_time(float raw_time) {
    // Credit [Nematode](https://github.com/ckgt/NemaTode): GPSTimestamp::setTime
    const int32_t raw_ts = int32_t(std::trunc(raw_time));

    const int32_t hours   = int32_t(std::trunc(raw_time / 10000.0f));
    const int32_t minutes = int32_t(std::trunc(float(raw_ts - hours * 10000) / 100.0f));
    const int32_t seconds = raw_ts - minutes * 100 + hours * 10000;

    return convert_time(hours, minutes, seconds, 0U);
}

} // namespace

void QCSP::CGPSGenerator::process(std::vector<int> & symbols) {

    const uint16_t curr_counter = this->counter++;

    std::ifstream tempf("/sys/class/thermal/thermal_zone0/temp");
    std::string   tempstr;
    getline(tempf, tempstr);
    const uint16_t temperature = std::stoul(tempstr) >> 1;

    std::unique_lock<std::mutex> lk_frame(this->m_frame);

    const float loc_lon  = this->lon;
    const float loc_lat  = this->lat;
    const float loc_time = this->raw_time;

    lk_frame.unlock();

    time_struct ts;
    if (use_localtime) {
        ts = get_local_time();
    } else {
        ts = get_gps_time(loc_time);
    }

    alignas(16) uint8_t bytes[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // clang-format off
    memcpy(bytes, &loc_lat, sizeof(float));
    memcpy(bytes + 4, &loc_lon, sizeof(float));
    memcpy(bytes + 8, &(ts.h_m_s), sizeof(uint16_t));
    memcpy(bytes + 10, &(ts.p12_cs), sizeof(uint8_t));
    memcpy(bytes + 11, &temperature, sizeof(uint16_t));
    memcpy(bytes + 13, &curr_counter, sizeof(uint16_t));
    // clang-format on

    memset(symbols.data(), 0, CSymbolGenerator::K * sizeof(int));

    constexpr uint8_t lowMask = (1U << std::min(CSymbolGenerator::p, 8U)) - 1U;

    unsigned i           = 0;
    unsigned bitsToWrite = CSymbolGenerator::p;
    for (unsigned charCnt = 0; charCnt < 15; charCnt++) {
        const uint8_t toWrite = bytes[charCnt];

        uint8_t rem = 8;
        assert(i < CSymbolGenerator::K);
        while (rem >= bitsToWrite) {
            const uint8_t bitOffset = rem - bitsToWrite;
            const uint8_t mask      = uint8_t(lowMask) << bitOffset;

            symbols[i] += int((toWrite & mask) >> bitOffset);
            assert(symbols[i] < (1 << CSymbolGenerator::p));
            i++;

            bitsToWrite = CSymbolGenerator::p; // New symbols[i] need full symbol
            rem         = bitOffset;
        }

        if (rem != 0) {
            const uint8_t offset = CSymbolGenerator::p - rem;
            const uint8_t mask   = (1U << rem) - 1U;

            symbols[i] = int((toWrite & mask) << offset);
            assert(symbols[i] < (1 << CSymbolGenerator::p));
            bitsToWrite = offset;
        }
    }

#if defined(DEBUG) && VERBOSE > 1
    constexpr uint8_t bytes_in_frame = CSymbolGenerator::p * CSymbolGenerator::K / 8;
    printf("%10.5f N %10.5f W %02d:%02d:%02d.%02d %05ddeg no %05d\n",
           loc_lat,
           loc_lon,
           hours,
           minutes,
           seconds,
           cents,
           temperature * 2,
           curr_counter);
    const bool loaded_p12 = (*((uint8_t *) (bytes + 10)) & 0x80) == 0x80;
    printf("%10.5f N %10.5f W %02d:%02d:%02d.%02d %05ddeg no %05d\n",
           *((float *) bytes),
           *((float *) (bytes + 4)),
           (*((uint16_t *) (bytes + 8)) >> 12) + (loaded_p12 ? 12 : 0),
           (*((uint16_t *) (bytes + 8)) >> 6) & 0x3F,
           (*((uint16_t *) (bytes + 8)) >> 0) & 0x3F,
           (*((uint8_t *) (bytes + 10)) & 0x7F),
           *((uint16_t *) (bytes + 11)) * 2,
           *((uint16_t *) (bytes + 13)));
    for (int i = 0; i < bytes_in_frame; i++) {
        printf(" %02x", bytes[i]);
    }
    cout << endl;
    for (unsigned i = 0; i < CSymbolGenerator::K; i++) {
        printf("    %02x ", symbols[i]);
    }
    cout << endl;
#if VERBOSE > 2
    bool check_bits[CSymbolGenerator::p * CSymbolGenerator::K];
    for (unsigned i = 0; i < CSymbolGenerator::K; i++) {
        for (unsigned j = 0; j < CSymbolGenerator::p; j++) {
            const uint8_t mask                      = 1 << (CSymbolGenerator::p - 1 - j);
            check_bits[i * CSymbolGenerator::p + j] = (symbols[i] & mask) == mask;
        }
    }

    for (unsigned i = 0; i < CSymbolGenerator::p * CSymbolGenerator::K; i++) {
        printf("%01u", check_bits[i]);
        if ((i + 1) % CSymbolGenerator::p == 0) {
            printf(" ");
        }
    }
    cout << endl;

    uint8_t check_char[bytes_in_frame] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (unsigned i = 0; i < bytes_in_frame; i++) {
        for (unsigned j = 0; j < 8; j++) {
            const uint8_t shift = (8 - 1 - j);
            check_char[i] += uint8_t(check_bits[i * 8 + j]) << shift;
        }
    }

    const bool wrld_p12 = (*((uint8_t *) (check_char + 10)) & 0x80) == 0x80;
    printf("%10.5f N %10.5f W %02d:%02d:%02d.%02d %05ddeg no %05d\n",
           *((float *) check_char),
           *((float *) (check_char + 4)),
           (*((uint16_t *) (check_char + 8)) >> 12) + (wrld_p12 ? 12 : 0),
           (*((uint16_t *) (check_char + 8)) >> 6) & 0x3F,
           (*((uint16_t *) (check_char + 8)) >> 0) & 0x3F,
           (*((uint8_t *) (check_char + 10)) & 0x7F),
           *((uint16_t *) (check_char + 11)) * 2,
           *((uint16_t *) (check_char + 13)));
#endif
#endif
}

int QCSP::CGPSGenerator::launch() {
    this->running = true;
    this->_t      = new std::thread(&QCSP::CGPSGenerator::run, this);
    return EXIT_SUCCESS;
}

int QCSP::CGPSGenerator::join() {
    if (this->running || !(_t->joinable())) {
        return EXIT_FAILURE;
    }

    this->_t->join();
    return EXIT_SUCCESS;
}

int QCSP::CGPSGenerator::stop() {
    running = false;
    return this->_t->joinable() ? EXIT_SUCCESS : EXIT_FAILURE;
}

void QCSP::CGPSGenerator::safe_join() {
    if (this->join() == EXIT_FAILURE) {
        printf("yolo\n");
        if (this->stop() == EXIT_FAILURE) {
            std::cerr << "[ERROR] GPS thread is not joinable." << std::endl;
        } else {
            this->_t->join();
        }
    }
}
