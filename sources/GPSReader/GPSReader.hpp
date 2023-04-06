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

#ifndef _USRP_GPS_READER_HPP_
#define _USRP_GPS_READER_HPP_

#include <atomic>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <nmeaparse/nmea.h>
#include <thread>

#include "utilities/definitions.hpp"

namespace QCSP {

class CGPSReader {
private:
    nmea::NMEAParser * _parser;
    nmea::GPSService * _gps;

    const std::string _tty_path;
    std::ifstream     _tty_gps;

    std::thread *     _t;
    std::atomic<bool> running;

    int * frame;

    float      lat;
    float      lon;
    std::mutex m_frame;

    uint16_t counter;

    float get_latitude() const { return float(_gps->fix.latitude); }

    float get_longitude() const { return float(_gps->fix.longitude); }

    void run();

public:
    static constexpr const unsigned log2gf   = _LOG2GF_;
    static constexpr const unsigned kSymbols = _KSYMBOL_;

    int launch();
    int join();
    int stop();

    void get_symbol_frame(int * symbols);

    CGPSReader(const std::string & tty_gps = "/dev/ttyUSB0");

    virtual ~CGPSReader();
};

} // namespace QCSP

#endif // _USRP_GPS_READER_HPP_
