#include "CSymbolGenerator/CFileReaderGenerator/CFileReaderGenerator.hpp"

void QCSP::CFileReaderGenerator::open(const std::string & filepath) {
    if (file_stream()->is_open()) {
        file_stream()->close();
    }
    this->file_stream()->open(filepath);
    if (!file_stream()->is_open()) {
        throw "ERROR: cannot open " + filepath + ".";
    }
}

void QCSP::CFileReaderGenerator::close() {
    if (!file_stream()->is_open()) {
        throw "ERROR: double closing.";
    }
    this->file_stream()->close();
    if (file_stream()->is_open()) {
        throw "ERROR: cannot close.";
    }
}

QCSP::CFileReaderGenerator::CFileReaderGenerator()
    : CByteReaderGenerator() {
    byte_stream = new std::ifstream();
}

QCSP::CFileReaderGenerator::CFileReaderGenerator(const std::string & filepath)
    : CByteReaderGenerator() {
    byte_stream = new std::ifstream();
    this->open(filepath);
}

QCSP::CFileReaderGenerator::~CFileReaderGenerator() {
    this->close();
    delete byte_stream;
}