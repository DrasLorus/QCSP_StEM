#include "./CSymbolGenerator.hpp"

QCSP::generator_t QCSP::gen_from_string(const std::string & in) {
    // if (in == "random") {
    //     return RANDOM;
    // }
    if (in == "timer") {
        return GEN_TIMER;
    }
    if (in == "zero") {
        return GEN_ZERO;
    }
    if (in == "gps") {
        return GEN_GPS;
    }
    if (in == "file") {
        return GEN_FILE;
    }
    if (in == "stdin") {
        return GEN_CIN;
    }
    return GEN_UNKNOW;
}
