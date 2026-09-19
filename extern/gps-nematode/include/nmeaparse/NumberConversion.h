/*
 * NumberConversion.h
 *
 *  Created on: Aug 14, 2014
 *      Author: Cameron Karlsson
 *
 *  See the license file included with this source.
 */

#ifndef NUMBERCONVERSION_H_
#define NUMBERCONVERSION_H_

#include <cstdint>
#include <exception>
#include <sstream>
#include <string>

#include <nmeaparse/config.h>

namespace nmea {

class NumberConversionError : public std::exception {
public:
    std::string message;
    NumberConversionError(std::string msg)
        : message(msg) {};

    virtual ~NumberConversionError() {};

    const char * what() const NMEA_NOEXCEPT {
        return message.c_str();
    }
};

double  parseDouble(std::string s);
int64_t parseInt(std::string s, int radix = 10);

// void NumberConversion_test();

} // namespace nmea

#endif /* NUMBERCONVERSION_H_ */
