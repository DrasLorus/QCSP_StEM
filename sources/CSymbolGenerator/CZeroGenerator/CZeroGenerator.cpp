#include <cstring>

#include "CSymbolGenerator/CZeroGenerator/CZeroGenerator.hpp"

void QCSP::CZeroGenerator::process(std::vector<int> & symbols) {
    std::memset(symbols.data(), 0, message_size() * sizeof(int));
}
