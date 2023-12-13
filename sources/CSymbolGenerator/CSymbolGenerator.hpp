#ifndef _SYMBOL_GENERATOR_HPP_
#define _SYMBOL_GENERATOR_HPP_

#include <string>
#include <vector>

#include "utilities/definitions.hpp"
#include "utilities/structures.hpp"

namespace QCSP {

/**
 * @brief Helper funtion to get a generator type from a string
 * 
 * @param in a generator string
 * @return generator_t a type of generator
 */
generator_t gen_from_string(const std::string & in);

/**
 * @brief Abstract class defining the interface for all symbol generators
 * 
 */
class CSymbolGenerator {
public:
    static constexpr const unsigned p = _LOG2GF_;
    static constexpr const unsigned K = _KSYMBOL_;

    static constexpr unsigned message_size() { return K; }
    static constexpr unsigned symbol_size() { return p; }

    /**
     * @brief Execute the generator
     *
     * @param symbols generated symbols
     */
    virtual void process(std::vector<int> & symbols) = 0;
};

} // namespace QCSP

#endif // _SYMBOL_GENERATOR_HPP_
