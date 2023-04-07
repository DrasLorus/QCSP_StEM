/*
 * Copyright (c) 2020-2022 "Camille Monière"
 * [mailto:camille.moniere@univ-ubs.fr]
 *
 * This file is part of QC-Toolbox.
 *
 * QC-Toolbox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <atomic>
#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>

#include <boost/program_options.hpp>

#include <fftw3.h>
// #include <fftw3f.h>
#include <matio.h>

#include <uhd/exception.hpp>
#include <uhd/types/tune_request.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/utils/thread.hpp>

#include "CConvEngine/CHalfCorrEngine.hpp"
#include "threads/timer.hpp"
#include "threads/user_interface.hpp"
#include "utilities/hmi_functions.hpp"
#include "utilities/usrp_functions.hpp"

// #include "00-Common/CFftwWrapper/CFftwWrapper.hpp"
// #include "CFaLNWrapper/CFaLNTransmitter.hpp"

#include "GPSReader/GPSReader.hpp"

// using FaLN_X::CFrame;

namespace po = boost::program_options;

typedef enum {
    UNKNOW,
    RANDOM,
    TIMER,
    ZERO,
    GPS
} generator_t;

generator_t gen_from_string(const std::string & in) {
    if (in == "random") {
        return RANDOM;
    }
    if (in == "timer") {
        return TIMER;
    }
    if (in == "zero") {
        return ZERO;
    }
    if (in == "gps") {
        return GPS;
    }
    return UNKNOW;
}

template <typename Tin, typename Tout, uint32_t inSize>
void upsample8(const Tin * __restrict in, Tout * __restrict out) {
    // constexpr uint32_t zero_padding = sizeof(Tout) * 7;
    for (uint32_t u = 0; u < inSize; u++) {
        const uint64_t up_idx = u << 3;

        out[up_idx + 0] = in[u];
        out[up_idx + 1] = 0;
        out[up_idx + 2] = 0;
        out[up_idx + 3] = 0;
        out[up_idx + 4] = 0;
        out[up_idx + 5] = 0;
        out[up_idx + 6] = 0;
        out[up_idx + 7] = 0;
    }
}

class CFaLNTransmitterBase {
public:
    int *  output() { return nullptr; };
    size_t output_size() const { return 0; }
    void   process() {}

    CFaLNTransmitterBase() = default;
    CFaLNTransmitterBase(const std::vector<int8_t> &, const std::vector<int8_t> &) {}
    virtual ~CFaLNTransmitterBase() = default;
};

class CRealTransmitter : public CFaLNTransmitterBase {
public:
    CRealTransmitter(const std::vector<int8_t> &, const std::vector<int8_t> &) {}
};
class CZeroTransmitter : public CFaLNTransmitterBase {
public:
    CZeroTransmitter(const std::vector<int8_t> &, const std::vector<int8_t> &) {}
};
class CTimeTransmitter : public CFaLNTransmitterBase {
public:
    CTimeTransmitter(const std::vector<int8_t> &, const std::vector<int8_t> &) {}
};

template <unsigned N, unsigned q>
class CReadTransmitter : public CFaLNTransmitterBase {
public:
    void load_symbols(int *) {}
    CReadTransmitter(const std::vector<int8_t> &, const std::vector<int8_t> &) {}
};

int UHD_SAFE_MAIN(int argc, char ** argv) {

    uhd::set_thread_priority_safe();

    po::variables_map vm;

    if (QCSP::parse_user_input(argc, argv, vm) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    const std::string device_args = vm.at("device").as<std::string>();
    const std::string ant         = vm.at("antenna").as<std::string>();
    const double      rate        = vm.at("rate").as<double>();
    const double      freq        = vm.at("freq").as<double>();
    const double      gain        = vm.at("gain").as<double>();

    const size_t inter_delay = (size_t) std::max(ceil(vm.at("inter-delay").as<double>()), 0.);

    const size_t max_count     = vm.at("count").as<unsigned>();
    const bool   count_limited = max_count > 0;

    const double ttl_us       = ceil(vm.at("duration").as<double>());
    const bool   time_limited = ttl_us > 0;

    const bool to_file     = vm.count("to-file");
    const bool save_frames = vm.count("save-frames");

    const bool no_ui = vm.count("no-ui");

    const generator_t gen_type = gen_from_string(vm.at("generator").as<std::string>());

    uhd::usrp::multi_usrp::sptr emitter_usrp;
    uhd::tx_streamer::sptr      send_stream;

    const std::string gps_tty = vm.at("tty").as<std::string>();
    if (gen_type == GPS) {
        if (FILE * test = fopen(gps_tty.c_str(), "r")) {
            fclose(test);
        } else {
            throw std::runtime_error("while opening the GPS tty " + gps_tty + ":\n    " + std::string(strerror(errno)));
            return EXIT_FAILURE;
        }
    }

    if (!to_file) {
        QCSP::init_usrp(device_args, vm, ant, rate, freq, gain, emitter_usrp, send_stream);
    }

    FILE * file_frames = nullptr;
    if (save_frames) {
        file_frames = fopen("saved_frames.bin", "wb");
    }

    unsigned int                     n_frame;
    unsigned int                     n_s;
    std::vector<int8_t>              pn;
    std::vector<int8_t>              best_N;
    std::vector<std::complex<float>> h_filter;

    QCSP::load_settings("../data/parameters_20210903.mat", n_frame, n_s, pn, best_N, h_filter);
    // CFrame frame(n_frame, _KSYMBOL_, _LOG2GF_, 1, pn.size());

    // val = [zeros(1, 88), kron((randi([0, 1], 1, 60*64) .* 2 - 1), [1 0 0 0 0 0 0 0]) , zeros(1,88)]

    std::shared_ptr<CFaLNTransmitterBase> generator;
    // if (vm.count("timer-generator")) {
    switch (gen_type) {
        case RANDOM:
            generator = std::make_shared<CRealTransmitter>(pn, best_N);
            break;
        case TIMER:
            generator = std::make_shared<CTimeTransmitter>(pn, best_N);
            break;
        case ZERO:
            generator = std::make_shared<CZeroTransmitter>(pn, best_N);
            break;
        case GPS:
            generator = std::make_shared<CReadTransmitter<QCSP::_KSYMBOL_, QCSP::_LOG2GF_>>(pn, best_N);
            break;
        default:
            std::cerr << "Error: generator type is unknown." << std::endl;
            exit(EXIT_FAILURE);
    }

    const int * const gen_out_int = generator->output();
    constexpr size_t  gen_outsize = QCSP::_NSYMBOL_ * QCSP::_GF_;
    if (gen_outsize != generator->output_size()) {
        std::cerr << "Error: generator->output_size() is expected to be _NSYMBOL_ * _GF_!" << std::endl;
        exit(EXIT_FAILURE);
    }

    const size_t conv_size = gen_outsize * 8 + (h_filter.size() - 1) * 2;

    if (bool(file_frames)) {
        fwrite(&conv_size, sizeof(size_t), 1, file_frames);
    }

    QCSP::CHalfCorrEngine<float> * conv_engine;
    ;

    std::vector<int8_t> gen_char_out(gen_outsize, 0);
    std::vector<int8_t> gen_upsp_out(conv_size, 0);

    std::vector<int8_t>::iterator data_beg_upsp = gen_upsp_out.begin() + (h_filter.size() - 1);

    std::vector<std::complex<float>> gen_cpx_out(conv_size, 0);
    std::vector<std::complex<float>> filtered_data(conv_size, 0);
    std::vector<std::complex<float>> buffer(conv_size, 0);

    float * const ptr_raw_fdata  = (float *) filtered_data.data();
    float * const ptr_raw_buffer = (float *) buffer.data();

    do {
        std::vector<std::complex<float>> h_filter_cpx(conv_size, 0);
        std::copy(h_filter.cbegin(), h_filter.cend(), h_filter_cpx.begin());

        conv_engine = new QCSP::CHalfCorrEngine<float>(conv_size, h_filter_cpx, FFTW_MEASURE);
    } while (0);

    size_t cnt = 0;

    std::atomic<bool> bRunning(true);
    std::atomic<bool> bTimeNotReached(true);

    QCSP::CGPSReader * gps_gen = nullptr;
    int *              symbols = nullptr;
    if (gen_type == GPS) {
        symbols = new int[QCSP::_KSYMBOL_];
        memset(symbols, 0, QCSP::_KSYMBOL_ * sizeof(int));
        gps_gen = new QCSP::CGPSReader(gps_tty);
        gps_gen->launch();
    }

    QCSP::ui_arg_t ui_arg = {std::ref(bRunning), no_ui};
    pthread_t      ui_tid = 0;
    pthread_create(&ui_tid, nullptr, &QCSP::user_interface_run, &ui_arg);

    QCSP::timer_arg_t timer_arg = {time_limited, ttl_us, std::ref(bTimeNotReached)};
    pthread_t         timer_tid = 0;
    pthread_create(&timer_tid, nullptr, &QCSP::timer_run, &timer_arg);

    const double frame_time = 1 / rate * 1e6 * double(conv_size);
    const size_t min_size   = size_t(ceil(frame_time * 2));
    const size_t true_delay = std::max(min_size, inter_delay - conv_size); // True inter delay is 2 frames OR requested delay minus 1 frame

    bool       bCountNotReached = true;
    const auto wait_time        = std::chrono::microseconds(true_delay);

    while (bRunning && bCountNotReached && bTimeNotReached) {

        if (bool(gps_gen)) {
            gps_gen->get_symbol_frame(symbols);
            std::dynamic_pointer_cast<CReadTransmitter<QCSP::_KSYMBOL_, QCSP::_LOG2GF_>>(generator)->load_symbols(symbols);
        }

        generator->process();

        std::copy(gen_out_int, gen_out_int + gen_outsize, gen_char_out.begin());
        upsample8<int8_t, int8_t, QCSP::_NSYMBOL_ * QCSP::_GF_>(gen_char_out.data(), data_beg_upsp.base());
        copy(gen_upsp_out.begin(), gen_upsp_out.begin() + conv_size, gen_cpx_out.begin());

        conv_engine->process(gen_cpx_out, filtered_data);

        for (size_t sz = 0; sz < conv_size * 2; sz += 2) {
            // Remove unnecessary imaginary parts introduced by FFT
            ptr_raw_buffer[sz]     = ptr_raw_fdata[sz];
            ptr_raw_buffer[sz + 1] = 0;
        }

        if (to_file) {
            QCSP::write_to_file<std::complex<float>>("dump_file.bin", buffer);
        } else {
            QCSP::send_from_memory<std::complex<float>>(send_stream, buffer);
        }

        if (bool(file_frames)) {
            fwrite(ptr_raw_buffer, sizeof(float), conv_size * 2, file_frames);
        }

        std::this_thread::sleep_for(wait_time);

        const bool tmp_cond = ++cnt < max_count;
        bCountNotReached    = (count_limited ? tmp_cond : true);
    }

    if (!bCountNotReached) {
        std::cout << "Transmission finished." << std::endl;
    }

    if (bRunning && !no_ui) {
        bRunning = false;
        pthread_cancel(ui_tid);
    }
    pthread_join(ui_tid, nullptr);

    if (bTimeNotReached && time_limited) {
        pthread_cancel(timer_tid);
    }
    pthread_join(timer_tid, nullptr);

    if (bool(gps_gen)) {
        gps_gen->stop();
        gps_gen->join();
        delete gps_gen;
        delete[] symbols;
    }

    delete conv_engine;

    if (bool(file_frames)) {
        fclose(file_frames);
    }

    return EXIT_SUCCESS;
}
