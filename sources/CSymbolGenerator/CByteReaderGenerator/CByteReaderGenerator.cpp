#include "CSymbolGenerator/CByteReaderGenerator/CByteReaderGenerator.hpp"

void QCSP::CByteReaderGenerator::open(const std::string & filepath) {
    if (file_stream.is_open()) {
        file_stream.close();
    }
    this->file_stream.open(filepath);
    if (!file_stream.is_open()) {
        throw "ERROR: cannot open " + filepath + ".";
    }
}

void QCSP::CByteReaderGenerator::close() {
    if (!file_stream.is_open()) {
        throw "ERROR: double closing.";
    }
    this->file_stream.close();
    if (file_stream.is_open()) {
        throw "ERROR: cannot close.";
    }
}

QCSP::CByteReaderGenerator::CByteReaderGenerator(const std::string & filepath)
    : CSymbolGenerator() {
    this->open(filepath);
}

QCSP::CByteReaderGenerator::~CByteReaderGenerator(){
    this->close();
}