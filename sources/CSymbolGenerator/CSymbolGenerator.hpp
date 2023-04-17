#ifndef _SYMBOL_GENERATOR_HPP_
#define _SYMBOL_GENERATOR_HPP_

#include <string>
#include <vector>

#include "utilities/definitions.hpp"
#include "utilities/structures.hpp"

namespace QCSP {

generator_t gen_from_string(const std::string & in);

class CSymbolGenerator {
public:
    static constexpr const unsigned p = _LOG2GF_;
    static constexpr const unsigned K = _KSYMBOL_;

    static constexpr unsigned message_size() { return K; }
    static constexpr unsigned symbol_size() { return p; }

    virtual void process(std::vector<int> & symbols) = 0;
};

} // namespace QCSP

#endif // _SYMBOL_GENERATOR_HPP_
