#include "./CQCSPModulator.hpp"

QCSP::modulator_t QCSP::mod_from_string(const std::string & in) {
    if (in == "nopc") {
        return MOD_NOPC;
    }
    // if (in == "zero") {
    //     return MOD_ZERO;
    // }
    if (in == "real") {
        return MOD_REAL;
    }
    return MOD_UNKNOW;
}
