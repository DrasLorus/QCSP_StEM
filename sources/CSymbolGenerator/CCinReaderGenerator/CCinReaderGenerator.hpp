/**
 * @file CCinReaderGenerator.hpp
 * @author Camille Monière (camille.moniere@univ-ubs.fr)
 * @brief
 * @version 0.1
 * @date 2023-12-13
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef _C_CIN_READER_GENERATOR_HPP_
#define _C_CIN_READER_GENERATOR_HPP_

#include "CSymbolGenerator/CByteReaderGenerator/CByteReaderGenerator.hpp"

#include <fstream>
#include <iostream>

namespace QCSP {

/**
 * @brief Implement a CByteReaderGenerator specialized in reading the standard input stream
 *
 */
class CCinReaderGenerator : public CByteReaderGenerator {
protected:
    virtual bool is_implemented() const final { return true; };

public:
    virtual void process(std::vector<uint_gf_t> & symbols) override;

    /**
     * @brief Construct a new CCinReaderGenerator object
     *
     */
    CCinReaderGenerator();

    /**
     * @brief Destroy the CCinReaderGenerator object
     *
     */
    virtual ~CCinReaderGenerator() = default;
};

} // namespace QCSP

#endif // _C_CIN_READER_GENERATOR_HPP_
