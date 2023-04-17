#include "./CQCSPModulator.hpp"

QCSP::modulator_t QCSP::mod_from_string(const std::string & in) {
    // if (in == "fake") {
    //     return MOD_FAKE;
    // }
    // if (in == "zero") {
    //     return MOD_ZERO;
    // }
    if (in == "real") {
        return MOD_REAL;
    }
    return MOD_UNKNOW;
}
