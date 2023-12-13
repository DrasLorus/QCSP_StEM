
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
    virtual void process(std::vector<int> & symbols) override;

    CCinReaderGenerator();
    virtual ~CCinReaderGenerator() = default;
};

} // namespace QCSP

#endif // _C_CIN_READER_GENERATOR_HPP_
