#include "CSymbolGenerator/CByteReaderGenerator/CByteReaderGenerator.hpp"

#include "utilities/conversions.hpp"
#include <cstring>

QCSP::CByteReaderGenerator::CByteReaderGenerator()
    : CSymbolGenerator(),
      buffer(nb_bytes, 0) {
}

void QCSP::CByteReaderGenerator::process(std::vector<int> & symbols) {
    static char * const ptr_buffer = this->buffer.data();

    std::memset(ptr_buffer, 0, nb_bytes); // Reset buffer

    if (this->good() && !(this->eof())) {
        byte_stream->read(ptr_buffer, nb_bytes);
    }

    bytes_to_int<CSymbolGenerator::p, CSymbolGenerator::K>(buffer, symbols);
}
