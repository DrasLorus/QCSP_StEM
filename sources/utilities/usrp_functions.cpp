#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include <uhd/exception.hpp>
#include <uhd/types/tune_request.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/utils/thread.hpp>

namespace po = boost::program_options;

namespace QCSP {

void check_usrp(uhd::device_addr_t device_args = std::string("")) {

    // create a usrp device
    std::cout << std::endl;
    std::cout << "Creating the usrp device with: '" << device_args.to_string() << "'" << std::endl;
    uhd::usrp::multi_usrp::sptr usrp = uhd::usrp::multi_usrp::make(device_args);

    std::cout << "Using Device:\n"
              << usrp->get_pp_string() << std::endl;

    std::cout << "Subdev specification: " << std::endl;
    for (unsigned u = 0; u < usrp->get_num_mboards(); u++) {
        std::string              mboard  = usrp->get_mboard_name(u);
        uhd::usrp::subdev_spec_t subdevs = usrp->get_tx_subdev_spec(u);
        std::cout << "Motherboard " << u << " '" << mboard << "':" << std::endl;
        std::cout << subdevs.to_pp_string() << std::endl;
    }

    auto   ratesrg = usrp->get_tx_rates();
    size_t prec    = std::cout.precision();
    std::cout.precision(3);
    std::cout << "Rate range:         [ "
              << std::setw(8) << ratesrg.start() / 1e6 << " : "
              << std::setw(8) << ratesrg.step() / 1e6 << " : "
              << std::setw(8) << ratesrg.stop() / 1e6 << " ] Msps" << std::endl;

    auto freqrg = usrp->get_tx_freq_range();
    std::cout << "RF frequency range: [ "
              << std::setw(8) << freqrg.start() / 1e6 << " : "
              << std::setw(8) << freqrg.step() / 1e6 << " : "
              << std::setw(8) << freqrg.stop() / 1e6 << " ] MHz" << std::endl;

    auto gainrg = usrp->get_tx_gain_range();
    std::cout << "Gain range:         [ "
              << std::setw(8) << gainrg.start() << " : "
              << std::setw(8) << gainrg.step() << " : "
              << std::setw(8) << gainrg.stop() << " ] dB" << std::endl;

    auto bwrg = usrp->get_tx_bandwidth_range();
    std::cout << "Bandwidth range:    [ "
              << std::setw(8) << bwrg.start() / 1e6 << " : "
              << std::setw(8) << bwrg.step() / 1e6 << " : "
              << std::setw(8) << bwrg.stop() / 1e6 << " ] MHz" << std::endl;

    // get antennas
    std::cout << "Available antennas: ";
    for (auto && antenna : usrp->get_tx_antennas()) {
        std::cout << antenna + " ";
    }
    std::cout.precision(prec);
    std::cout << std::endl;
}

using vm_t     = po::variables_map;
using p_usrp_t = uhd::usrp::multi_usrp::sptr;
using p_tx_t   = uhd::tx_streamer::sptr;

void init_usrp(const std::string & device_args,
               const vm_t &        vm,
               const std::string & ant,
               double              rate,
               double              freq,
               double              gain,
               p_usrp_t &          emitter_usrp,
               p_tx_t &            send_stream) {
    emitter_usrp = uhd::usrp::multi_usrp::make(device_args);

    if (vm.count("clock-source")) {
        emitter_usrp->set_clock_source(vm.at("clock-source").as<std::string>());
    }

    if (vm.count("subdev")) {
        emitter_usrp->set_tx_subdev_spec(vm.at("subdev").as<std::string>());
    }

    emitter_usrp->set_tx_antenna(ant);

    emitter_usrp->set_tx_rate(rate);
    emitter_usrp->set_tx_freq(freq);
    emitter_usrp->set_tx_gain(gain);
    if (vm.count("bandwidth")) {
        emitter_usrp->set_tx_bandwidth(vm.at("bandwidth").as<double>());
    }

    emitter_usrp->set_time_now(0.0);
    const int prec = std::cout.precision();
    std::cout << "Using " << emitter_usrp->get_pp_string()
              << "\n  Antenna:    " << emitter_usrp->get_tx_antenna()
              << "\n  Rate (Msps):      " << std::setprecision(7) << emitter_usrp->get_tx_rate() * 1e-6
              << "\n  Frequency (MHz):  " << std::setprecision(7) << emitter_usrp->get_tx_freq() * 1e-6
              << "\n  Bandwidth (MHz):  " << std::setprecision(7) << emitter_usrp->get_tx_freq() * 1e-6
              << "\n  Gain (dB):        " << std::setprecision(7) << emitter_usrp->get_tx_gain()
              << std::endl;
    std::cout.precision(prec);

    uhd::stream_args_t stream_args(vm.at("cpu-format").as<std::string>(), vm.at("otw-format").as<std::string>());
    send_stream = emitter_usrp->get_tx_stream(stream_args);
}

} // namespace QCSP
