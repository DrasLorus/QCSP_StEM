#ifndef _ZERO_GENERATOR_HPP_
#define _ZERO_GENERATOR_HPP_

#include <vector>

#include "CSymbolGenerator/CSymbolGenerator.hpp"

namespace QCSP {

/**
 * @brief Generate null frames
 *
 */
class CZeroGenerator : public CSymbolGenerator {
public:
    virtual void process(std::vector<uint_gf_t> & symbols) override;

    CZeroGenerator()          = default;
    virtual ~CZeroGenerator() = default;
};

} // namespace QCSP

#endif // _ZERO_GENERATOR_HPP_
