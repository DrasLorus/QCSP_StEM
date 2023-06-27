
#ifndef _C_CIN_READER_GENERATOR_HPP_
#define _C_CIN_READER_GENERATOR_HPP_

#include "CSymbolGenerator/CByteReaderGenerator/CByteReaderGenerator.hpp"

#include <fstream>
#include <iostream>

namespace QCSP {

class CCinReaderGenerator : public CByteReaderGenerator {
protected:
    virtual bool is_implemented() const final { return true; };

public:
    CCinReaderGenerator();
    virtual ~CCinReaderGenerator() = default;
};

} // namespace QCSP

#endif // _C_CIN_READER_GENERATOR_HPP_
