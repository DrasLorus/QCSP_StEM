#include "CSymbolGenerator/CCinReaderGenerator/CCinReaderGenerator.hpp"

#include "utilities/conversions.hpp"
#include <cstring>

QCSP::CCinReaderGenerator::CCinReaderGenerator()
    : CByteReaderGenerator() {
    byte_stream = &std::cin;
}
