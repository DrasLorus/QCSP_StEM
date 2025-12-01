#include <cstring>

#include "CSymbolGenerator/CZeroGenerator/CZeroGenerator.hpp"

void QCSP::CZeroGenerator::process(std::vector<uint_gf_t> & symbols) {
    std::memset(symbols.data(), 0, message_size() * sizeof(uint_gf_t));
}
