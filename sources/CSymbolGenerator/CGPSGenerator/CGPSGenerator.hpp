/**
 * @file CGPSGenerator.hpp
 * @author Camille Monière (camille.moniere@univ-ubs.fr)
 * @brief
 * @version 0.1
 * @date 2023-04-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef _QCSP_GPS_GENERATOR_HPP_
#define _QCSP_GPS_GENERATOR_HPP_

#include <atomic>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <nmeaparse/nmea.h>
#include <thread>

#include "CSymbolGenerator/CSymbolGenerator.hpp"

namespace QCSP {

class CGPSGenerator : public CSymbolGenerator {
private:
    nmea::NMEAParser * _parser;
    nmea::GPSService * _gps;

    const std::string _tty_path;
    std::ifstream     _tty_gps;

    std::thread *     _t;
    std::atomic<bool> running;
    const bool        use_localtime;

    float      lat;
    float      lon;
    float      raw_time;
    std::mutex m_frame;

    uint16_t counter;

    float get_latitude() const { return float(this->_gps->fix.latitude); }

    float get_longitude() const { return float(this->_gps->fix.longitude); }

    void run();
    int  join();
    int  stop();

public:
    int  launch();
    void safe_join();

    virtual void process(std::vector<int> & symbols) override;

    CGPSGenerator(const std::string & tty_gps = "/dev/ttyUSB0", bool localtime = true, bool do_launch = true);

    virtual ~CGPSGenerator();
};

} // namespace QCSP

#endif // _QCSP_GPS_GENERATOR_HPP_
