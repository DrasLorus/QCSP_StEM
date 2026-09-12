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

#include <uhd/utils/safe_main.hpp>
#include <uhd/utils/thread.hpp>

#include "CConvEngine/CHalfCorrEngine.hpp"
#include "CQCSPModulator/CCompleteModulator/CCompleteModulator.hpp"
#include "CQCSPModulator/CFakeEncoderModulator/CFakeEncoderModulator.hpp"
#include "CQCSPModulator/CQCSPModulator.hpp"
#include "CSymbolGenerator/CByteReaderGenerator/CByteReaderGenerator.hpp"
#include "CSymbolGenerator/CCinReaderGenerator/CCinReaderGenerator.hpp"
#include "CSymbolGenerator/CFileReaderGenerator/CFileReaderGenerator.hpp"
#include "CSymbolGenerator/CGPSGenerator/CGPSGenerator.hpp"
#include "CSymbolGenerator/CTimerGenerator/CTimerGenerator.hpp"
#include "CSymbolGenerator/CZeroGenerator/CZeroGenerator.hpp"
#include "threads/timer.hpp"
#include "threads/user_interface.hpp"
#include "utilities/definitions.hpp"
#include "utilities/hmi_functions.hpp"
#include "utilities/usrp_functions.hpp"

// using FaLN_X::CFrame;

namespace po = boost::program_options;

template <typename Tin, typename Tout, uint32_t inSize>
void upsample8(const Tin * __restrict in, Tout * __restrict out) {
    // constexpr uint32_t zero_padding = sizeof(Tout) * 7;
    for (uint32_t u = 0; u < inSize; u++) {
        const uint64_t up_idx = u << 3;

        out[up_idx + 0] = Tout(in[u]);
        out[up_idx + 1] = 0;
        out[up_idx + 2] = 0;
        out[up_idx + 3] = 0;
        out[up_idx + 4] = 0;
        out[up_idx + 5] = 0;
        out[up_idx + 6] = 0;
        out[up_idx + 7] = 0;
    }
}

int UHD_SAFE_MAIN(int argc, char * argv[]) {

    // uhd::set_thread_priority_safe();
    po::variables_map        vm;
    QCSP::emitter_parameters prm;

    if (QCSP::parse_user_input(argc, argv, vm) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    QCSP::parse_vm(vm, prm);

    uhd::usrp::multi_usrp::sptr usrp_transmitter;
    uhd::tx_streamer::sptr      send_stream;
    if (!prm.to_file) {
        QCSP::init_usrp(prm, usrp_transmitter, send_stream);
    }

    FILE * file_frames = nullptr;
    if (prm.save_frames) {
        file_frames = fopen("saved_frames.bin", "wb");
    }

    unsigned int                     n_frame;
    unsigned int                     n_s;
    std::vector<int>                 pn;
    std::vector<int>                 best_N;
    std::vector<std::complex<float>> h_filter;

    QCSP::load_settings(prm.param_file, n_frame, n_s, pn, best_N, h_filter);

    std::shared_ptr<QCSP::CSymbolGenerator> generator;
    switch (prm.gen_type) {
        // case QCSP::GEN_RANDOM:
        //     generator = std::make_shared<CRandomGenerator>(n_frame, n_s);
        //     break;
        case QCSP::GEN_TIMER:
            generator = std::make_shared<QCSP::CTimerGenerator>();
            break;
        case QCSP::GEN_ZERO:
            generator = std::make_shared<QCSP::CZeroGenerator>();
            break;
        case QCSP::GEN_GPS:
            generator = std::make_shared<QCSP::CGPSGenerator>(prm.gps_tty, false, true);
            break;
        case QCSP::GEN_FILE:
            generator = std::make_shared<QCSP::CFileReaderGenerator>(prm.input_file);
            break;
        case QCSP::GEN_CIN:
            generator = std::make_shared<QCSP::CCinReaderGenerator>();
            break;
        default:
            std::cerr << "Error: generator type is unknown." << std::endl;
            exit(EXIT_FAILURE);
    }

    std::shared_ptr<QCSP::CQCSPModulator> modulator;
    switch (prm.mod_type) {
        // case QCSP::MOD_ZERO:
        //     generator = std::make_shared<CZeroTransmitter>(n_frame, n_s);
        //     break;
        case QCSP::MOD_NOPC:
            modulator = std::make_shared<QCSP::CFakeEncoderModulator>(pn, best_N);
            break;
        case QCSP::MOD_REAL:
            modulator = std::make_shared<QCSP::CCompleteModulator>(pn, best_N);
            break;
        default:
            std::cerr << "Error: modulator type is unknown." << std::endl;
            exit(EXIT_FAILURE);
    }

    constexpr size_t qcsp_message_size = QCSP::CQCSPModulator::message_size();
    constexpr size_t qcsp_frame_size   = QCSP::CQCSPModulator::frame_size();

    const size_t data_buffer_size = qcsp_frame_size * 8 + (h_filter.size() - 1) * 2;
    const size_t conv_size        = 2U << unsigned(std::ceil(std::log2(float(data_buffer_size))));

    if (bool(file_frames)) {
        fwrite(&conv_size, sizeof(size_t), 1, file_frames);
    }

    QCSP::CHalfCorrEngine<float> * conv_engine;

    std::vector<QCSP::uint_gf_t> message(qcsp_message_size, 0);
    std::vector<int>             qcsp_frame(qcsp_frame_size, 0);
    std::vector<int8_t>          frame_upsp_int8(conv_size, 0);

    std::vector<int8_t>::iterator data_beg_upsp = frame_upsp_int8.begin() + (h_filter.size() - 1);

    std::vector<std::complex<float>> frame_cpx(conv_size, 0);
    std::vector<std::complex<float>> filtered_data(conv_size, 0);
    std::vector<std::complex<float>> buffer(data_buffer_size, 0);

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
    std::atomic<bool> bGood(true);

    QCSP::ui_arg_t ui_arg = {std::ref(bRunning), prm.no_ui};
    pthread_t      ui_tid = 0;
    pthread_create(&ui_tid, nullptr, &QCSP::user_interface_run, &ui_arg);

    QCSP::timer_arg_t timer_arg = {prm.time_limited, prm.ttl_us, std::ref(bTimeNotReached)};
    pthread_t         timer_tid = 0;
    pthread_create(&timer_tid, nullptr, &QCSP::timer_run, &timer_arg);

    const double frame_latency = 1 / prm.rate * 1e6 * double(data_buffer_size);
    const size_t frame_us      = size_t(std::ceil(frame_latency));
    const size_t true_delay    = prm.inter_delay > frame_us ? (prm.inter_delay - frame_us) : 0;
    if (true_delay < frame_us) { // True inter delay should be 2 frames OR requested delay minus 1 frame
        QCSP::warning_stream()
            << "The delay of " << true_delay << " us between two successive frames does not allow reliable detection.\n"
            << QCSP::line_filler() << "The minimum is " << frame_us << " us (sending time of a frame).\n"
            << QCSP::line_filler() << "Specify an inter-delay of " << frame_us * 2 << " us to ensure reliable detection." << std::endl;
    }

    bool       bCountNotReached = true;
    const bool stream_generator = (prm.gen_type == QCSP::GEN_CIN) || (prm.gen_type == QCSP::GEN_FILE);

    const std::chrono::microseconds wait_time = std::chrono::microseconds(true_delay);

    while (bRunning && bCountNotReached && bTimeNotReached && bGood) {

        generator->process(message);
        if (stream_generator) {
            bGood = static_cast<const QCSP::CByteReaderGenerator *>(generator.get())->good();
            if (!bGood) {
                break;
            }
        }
        modulator->process(message, qcsp_frame);

        // std::cout << std::endl;
        // for (int i = 0; i < qcsp_frame_size; i++)
        // 	std::cout << qcsp_frame[i] << " ";
        // std::cout << std::endl;

        // std::copy(qcsp_frame.begin(), qcsp_frame.begin() + qcsp_frame_size, frame_int8.begin());
        upsample8<int, int8_t, QCSP::_NSYMBOL_ * QCSP::_GF_>(qcsp_frame.data(), data_beg_upsp.base());
        copy(frame_upsp_int8.begin(), frame_upsp_int8.begin() + conv_size, frame_cpx.begin());

        conv_engine->process(frame_cpx, filtered_data);
        for (size_t sz = 0; sz < data_buffer_size * 2; sz += 2) {
            static const float scaling = 5.f / float(conv_size);
            // Remove unnecessary imaginary parts introduced by FFT and add scaling
            ptr_raw_buffer[sz]     = ptr_raw_fdata[sz] * scaling;
            ptr_raw_buffer[sz + 1] = 0;
        }

        if (prm.to_file) {
            QCSP::write_to_file<std::complex<float>>(prm.output_file, buffer);
        } else {
            QCSP::send_from_memory<std::complex<float>>(send_stream, buffer);
        }

        if (bool(file_frames)) {
            fwrite(ptr_raw_buffer, sizeof(float), data_buffer_size * 2, file_frames);
        }

        std::this_thread::sleep_for(wait_time);

        const bool tmp_cond = ++cnt < prm.max_count;
        bCountNotReached    = (prm.count_limited ? tmp_cond : true);
    }

    if (!bGood && stream_generator) {
        if (static_cast<const QCSP::CByteReaderGenerator *>(generator.get())->eof()) {
            std::cout << "End of file reached." << std::endl;
        } else {
            std::cout << "Unknown error encountered with the input." << std::endl;
        }
    }

    if (!bCountNotReached) {
        std::cout << "Transmission finished." << std::endl;
    }

    if (bRunning && !prm.no_ui) {
        bRunning = false;
        pthread_cancel(ui_tid);
    }
    pthread_join(ui_tid, nullptr);

    if (bTimeNotReached && prm.time_limited) {
        pthread_cancel(timer_tid);
    }
    pthread_join(timer_tid, nullptr);

    delete conv_engine;

    if (bool(file_frames)) {
        fclose(file_frames);
    }

    return EXIT_SUCCESS;
}
