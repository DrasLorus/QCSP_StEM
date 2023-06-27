#include <iostream>

#include <boost/program_options.hpp>
#include <matio.h>

#include <string>
#include <uhd/exception.hpp>
#include <uhd/usrp/multi_usrp.hpp>

#include "CQCSPModulator/CQCSPModulator.hpp"
#include "CSymbolGenerator/CSymbolGenerator.hpp"

#include "./definitions.hpp"
#include "./hmi_functions.hpp"
#include "./usrp_functions.hpp"

namespace po = boost::program_options;

#define USAGE(desc) "Usage: qcsp_standalone_emitter <rate> <freq> <gain> <antenna> [options]\n"           \
                        << "Options:\n"                                                                   \
                        << desc << "\n\n"                                                                 \
                        << "Note: <rate>, <freq>, <gain> and <antenna> must be specified exactly once,\n" \
                        << "either passed as positional or dashed arguments.\n"

namespace {

void exit_if(bool condition, const std::string & message) {
    if (condition) {
        std::cerr << message << std::endl;
        exit(EXIT_FAILURE);
    }
}

template <class Exception>
void throw_if(bool condition, Exception && e) {
    if (condition) {
        throw e;
    }
}

} // namespace

int QCSP::parse_user_input(int argc, char * argv[], po::variables_map & vm) {

    po::options_description desc;
    desc.add_options()(
        "help,h", "Produce this message.")(
        "device,d", po::value<std::string>()->default_value(""), "USRP device identifier ('device_arg').\n"
                                                                 "Default: first encountered device.")(
        "subdev,s", po::value<std::string>(), "USRP subdevice identifier ('subdev').\n"
                                              "Default: None.")(
        "rate,r", po::value<double>()->required(), "Transmission rate ('tx_rate', sps).")(
        "freq,f", po::value<double>()->required(), "Central frequency ('tx_freq', Hz).")(
        "gain,g", po::value<double>()->required(), "Transmission gain ('tx_gain', dB).")(
        "antenna,a", po::value<std::string>()->required(), "Transmission antenna ('tx_antenna').")(
        "bandwidth,w", po::value<double>(), "Transmission bandwidth ('tx_bandwidth', Hz).\n"
                                            "Default: Automatic.")(
        "clock-source,s", po::value<double>(), "Clock reference source.\n"
                                               "Default: Automatic.")(
        "cpu-format", po::value<std::string>()->default_value("fc32"), "CPU Format, forwarded to the tx stream.\n"
                                                                       "Default: fc32 (complex<float>).")(
        "otw-format", po::value<std::string>()->default_value("sc16"), "Over-The-Wire Format, forwarded to the tx stream.\n"
                                                                       "Default: sc16 (Q16 I16).")(
        "count,n", po::value<unsigned>()->default_value(0), "Frame count to send.\n"
                                                            "If 0, run while authorized to.")(
        "duration,t", po::value<double>()->default_value(0), "Program time to live in microseconds.\n"
                                                             "If 0 or less, unlimited.\n"
                                                             "The value is truncated.")(
        "inter-delay", po::value<double>()->default_value(0), "Time between two burst in microseconds.\n"
                                                              "Values too low are ignored (A minimum interval of one buffer is hardcoded).\n"
                                                              "The value is ceiled.")(
        "to-file", "Write to a file instead of a USRP.")(
        "output-file", po::value<std::string>()->default_value("dump_file.bin"), "File to write to, when using '--to-file'.")(
        "save-frames", "Emit but also store frames locally")(
        "generator", po::value<std::string>()->default_value("gps"), "Set the generator used. You can choose between :" //// "Either 'random' (randomly generated bits),"
                                                                     " - 'timer' (to use the predictable CTimerTransmitter),"
                                                                     " - 'zero' (to send only zeroes),"
                                                                     " - 'gps' (binary payload with GPS position),"
                                                                     " - 'file' (read and send a file),"
                                                                     " - 'stdin' (read bytes from standard input).")(
        "modulator", po::value<std::string>()->default_value("real"), "Set the modulator used. Either 'real' (true QCSP modulator),"
                                                                      " or 'fake' (generate the same valid frame independently of the payload).")(
        "tty", po::value<std::string>(), "Set the TTY used to read GPS data. Only required with the 'gps' generator.")(
        "input-file", po::value<std::string>(), "Set the file used as input. Only required with the 'file' generator.")(
        "no-ui", "Disable the UI (program no longer cleanly stoppable by the user).")(
        "probe", "Look for all available USRP.");

    po::positional_options_description p;
    p.add("rate", 1).add("freq", 1).add("gain", 1).add("antenna", 1);

    try {
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        if (vm.count("help")) {
            std::cerr << USAGE(desc)
                      << std::endl;
            return EXIT_FAILURE;
        }

        if (vm.count("probe")) {
            uhd::device_addrs_t usrp_found = uhd::device::find(std::string());
            if (usrp_found.size() == 0) {
                std::cout << "No USRP found." << std::endl;
            } else {
                for (auto && device_args : usrp_found) {
                    check_usrp(device_args);
                }
            }
            return EXIT_FAILURE;
        }

        po::notify(vm);
    } catch (const std::exception & e) {
        std::cerr << "Error while parsing: " << e.what() << std::endl
                  << USAGE(desc)
                  << std::endl;
        exit(EXIT_FAILURE);
    } catch (...) {
        std::cerr << "Unknown Error! Terminating..." << std::endl;
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

/**
 * @brief cancellation point
 *
 */
void QCSP::load_settings(
    const std::string &                filename,
    unsigned &                         n_frame,
    unsigned &                         n_s,
    std::vector<int> &                 pn,
    std::vector<int> &                 best_N,
    std::vector<std::complex<float>> & h_filter) {
    mat_t * mat_params = Mat_Open(filename.c_str(), MAT_ACC_RDONLY);
    throw_if(mat_params == nullptr, std::runtime_error("Cannot open file " + filename));

    matvar_t * var_p = Mat_VarRead(mat_params, "n_frame");
    throw_if(mat_params == nullptr, std::runtime_error("Cannot open variable n_frame"));

    n_frame = unsigned(*reinterpret_cast<double *>(var_p->data));
    if (n_frame != _NSYMBOL_) {
        std::cerr << "ERROR - Extracted n_frame (" << n_frame
                  << ") != Expected n_frame (" << _NSYMBOL_ << ")\n"
                  << "Abort..." << std::endl;
        exit(EXIT_FAILURE);
    }
    Mat_VarFree(var_p);
    var_p = nullptr;

    var_p = Mat_VarRead(mat_params, "n_s");
    throw_if(mat_params == nullptr, std::runtime_error("Cannot open variable n_s"));

    n_s = unsigned(*reinterpret_cast<double *>(var_p->data));
    if (n_s != _GF_) {
        std::cerr << "ERROR - Extracted n_s (" << n_s
                  << ") != Expected n_s (" << _GF_ << ")\n"
                  << "Abort..." << std::endl;
        exit(EXIT_FAILURE);
    }
    Mat_VarFree(var_p);
    var_p = nullptr;

    var_p = Mat_VarRead(mat_params, "PN64");
    throw_if(mat_params == nullptr, std::runtime_error("Cannot open variable PN64"));

    pn = std::vector<int>((double *) var_p->data, (double *) var_p->data + var_p->dims[1]);
    if (pn.size() != n_s) {
        std::cerr << "ERROR - Current PN size (" << pn.size()
                  << ") != Expected size (" << n_s << ")\n"
                  << "Abort..." << std::endl;
        exit(EXIT_FAILURE);
    }
    Mat_VarFree(var_p);
    var_p = nullptr;

    var_p = Mat_VarRead(mat_params, "best_N");
    throw_if(mat_params == nullptr, std::runtime_error("Cannot open variable best_N"));

    best_N = std::vector<int>((double *) var_p->data, (double *) var_p->data + var_p->dims[1]);
    if (best_N.size() != n_frame) {
        std::cerr << "ERROR - Current best_N size (" << best_N.size()
                  << ") != Expected size (" << n_frame << ")\n"
                  << "Abort..." << std::endl;
        exit(EXIT_FAILURE);
    }
    Mat_VarFree(var_p);
    var_p = nullptr;

    var_p = Mat_VarRead(mat_params, "h_filter");
    throw_if(mat_params == nullptr, std::runtime_error("Cannot open variable h_filter"));

    h_filter = std::vector<std::complex<float>>((double *) var_p->data, (double *) var_p->data + var_p->dims[0]);

    Mat_VarFree(var_p);
    var_p = nullptr;

    exit_if(Mat_Close(mat_params), "Error closing " + filename);
}

void QCSP::parse_vm(const po::variables_map & vm, emitter_parameters & prm) {
    prm.device_args = vm.at("device").as<std::string>();
    prm.ant         = vm.at("antenna").as<std::string>();

    prm.rate = vm.at("rate").as<double>();
    prm.freq = vm.at("freq").as<double>();
    prm.gain = vm.at("gain").as<double>();

    prm.inter_delay = size_t(std::max(ceil(vm.at("inter-delay").as<double>()), 0.));

    const unsigned max_count = vm.at("count").as<unsigned>();
    prm.max_count            = max_count;
    prm.count_limited        = max_count > 0;

    const double ttl_us = ceil(vm.at("duration").as<double>());
    prm.ttl_us          = ttl_us;
    prm.time_limited    = ttl_us > 0;

    prm.to_file     = vm.count("to-file");
    prm.output_file = vm.at("output-file").as<std::string>();
    prm.save_frames = vm.count("save-frames");
    prm.no_ui       = vm.count("no-ui");

    prm.gen_type = QCSP::gen_from_string(vm.at("generator").as<std::string>());
    prm.mod_type = QCSP::mod_from_string(vm.at("modulator").as<std::string>());

    if (prm.gen_type == QCSP::GEN_GPS) {
        if (!vm.count("tty")) {
            throw std::invalid_argument("The tty argument is required with the 'gps' generator.'\nAdd '--tty </path/to/tty>' to specify it.");
        }
        const std::string & gps_tty = vm.at("tty").as<std::string>();
        if (FILE * test = fopen(gps_tty.c_str(), "r")) {
            fclose(test);
            prm.gps_tty = gps_tty;
        } else {
            throw std::runtime_error("while opening the GPS tty " + gps_tty + ":\n    " + std::string(strerror(errno)));
        }
    } else if (prm.gen_type == QCSP::GEN_FILE) {
        if (!vm.count("input-file")) {
            throw std::invalid_argument("The input file argument is required with the 'file' generator.\nAdd '--input-file </path/to/file>' to specify it.");
        }
        const std::string & input_file = vm.at("input-file").as<std::string>();
        if (FILE * test = fopen(input_file.c_str(), "r")) {
            fclose(test);
            prm.input_file = input_file;
        } else {
            throw std::runtime_error("while opening the file " + input_file + ":\n    " + std::string(strerror(errno)));
        }
    }

    const bool has_clock_source = vm.count("clock-source");
    prm.has_clock_source        = has_clock_source;
    if (has_clock_source) {
        prm.clock_source = vm.at("clock-source").as<std::string>();
    } else {
        prm.clock_source = "";
    }

    const bool has_subdev = vm.count("subdev");
    prm.has_subdev        = has_subdev;
    if (has_subdev) {
        prm.subdev = vm.at("subdev").as<std::string>();
    } else {
        prm.subdev = "";
    }

    const bool has_bandwidth = vm.count("bandwidth");
    prm.has_bandwidth        = has_bandwidth;
    if (has_bandwidth) {
        prm.bandwidth = vm.at("bandwidth").as<double>();
    } else {
        prm.bandwidth = -1.0;
    }

    prm.cpu_format = vm.at("cpu-format").as<std::string>();
    prm.otw_format = vm.at("otw-format").as<std::string>();
}
