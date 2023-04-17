/*
 * Copyright (c) 2012-2019 "Bordeaux INP, Bertrand LE GAL"
 * [http://legal.vvv.enseirb-matmeca.fr]
 *
 * Copyright (c) 2020-2022 "Camille Monière"
 * [mailto:camille.moniere@univ-ubs.fr]
 *
 * This file is part of FaLN-X.
 *
 * FaLN-X is free software: you can redistribute it and/or modify
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

#include <cassert>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "./CTimerGenerator.hpp"
#include "CSymbolGenerator/CSymbolGenerator.hpp"

#ifndef ARCH_STR
#if defined(__x86_64) || defined(__x86_64__) || defined(__amd64__) || defined(__amd64)
#define ARCH_STR "X"
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_X86_)
#define ARCH_STR "x"
#elif defined(__aarch64__)
#define ARCH_STR "A"
#elif defined(__arm__) || defined(__thumb__)
#define ARCH_STR "a"
#else
#define ARCH_STR "u"
#endif
#endif

QCSP::CTimerGenerator::CTimerGenerator()
    : counter(0) {
}

void QCSP::CTimerGenerator::process(std::vector<int> & message) {
    namespace ch = std::chrono;
    using clk    = ch::system_clock;

    constexpr const unsigned message_bit_size  = CSymbolGenerator::message_size();
    constexpr const unsigned message_byte_size = message_bit_size / 8;
    constexpr const bool     more_two_bytes    = CSymbolGenerator::p > 8;

    //  const uint8_t high_mask = (1U << unsigned(std::max(0, int(CSymbolGenerator::p) - 8))) - 1U;
    constexpr uint8_t low_mask = (1U << std::min(CSymbolGenerator::p, 8U)) - 1U;

    int * const src = message.data();
    memset(src, 0, CSymbolGenerator::K * sizeof(int));

    const ch::time_point<clk> now = clk::now();
    const std::time_t         t_c = clk::to_time_t(now);

    constexpr uint8_t time_str_size = 16;
    char              time_str[time_str_size];
    strftime(time_str, time_str_size, "%T", std::localtime(&t_c));

    char counter_str[time_str_size];
    sprintf(counter_str, "%05u|", this->counter);

    this->counter++;

    const std::string to_send = std::string(counter_str) + std::string(time_str) + ARCH_STR;
    if (to_send.size() > message_byte_size) {
        std::cerr << "[Warning] string is too long." << std::endl;
    }
    const char * char_to_send = to_send.c_str();

    if (more_two_bytes) {
        std::cerr << "[ERROR] 2B not ready yet." << std::endl;
        exit(EXIT_FAILURE);
    } else {
        unsigned i             = 0;
        unsigned bits_to_write = CSymbolGenerator::p;
        for (unsigned char_cnt = 0; char_cnt < message_byte_size; char_cnt++) {
            const uint8_t to_write = char_to_send[char_cnt];

            uint8_t rem = 8;
            assert(i < CSymbolGenerator::K);
            while (rem >= bits_to_write) {
                const uint8_t bit_offset = rem - bits_to_write;
                const uint8_t mask       = uint8_t(low_mask) << bit_offset;

                src[i++] += int((to_write & mask) >> bit_offset);

                bits_to_write = CSymbolGenerator::p; // New src[i] need full symbol
                rem           = bit_offset;
            }

            if (rem != 0) {
                const uint8_t offset = CSymbolGenerator::p - rem;
                const uint8_t mask   = (1U << rem) - 1U;

                src[i]        = int((to_write & mask) << offset);
                bits_to_write = offset;
            }
        }
    }
}
