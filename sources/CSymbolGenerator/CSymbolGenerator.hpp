#ifndef _SYMBOL_GENERATOR_HPP_
#define _SYMBOL_GENERATOR_HPP_

#include <string>
#include <vector>

#include "utilities/definitions.hpp"

namespace QCSP {

typedef enum {
    GEN_UNKNOW,
    // GEN_RANDOM,
    // GEN_TIMER,
    // GEN_ZERO,
    GEN_GPS
} generator_t;

generator_t gen_from_string(const std::string & in);


class CSymbolGenerator {
public:
    static constexpr const unsigned p = _LOG2GF_;
    static constexpr const unsigned K = _KSYMBOL_;

    constexpr std::size_t message_size() const { return K; }
    constexpr std::size_t symbol_size() const { return p; }

    virtual void process(std::vector<int> & symbols) = 0;
};

} // namespace QCSP

#endif // _SYMBOL_GENERATOR_HPP_
