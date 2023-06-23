
#ifndef _C_READ_GENERATOR_HPP_
#define _C_READ_GENERATOR_HPP_

#include "CSymbolGenerator/CSymbolGenerator.hpp"

#include <fstream>
#include <iostream>

namespace QCSP {

class CByteReaderGenerator : public CSymbolGenerator {
private:
    std::ifstream file_stream;

public:
    bool is_open() const { return this->file_stream.is_open(); }
    bool end_of_file() const { return this->file_stream.eof(); }
    void open(const std::string & filepath);
    void close();

    virtual void process(std::vector<int> & symbols) override;

    CByteReaderGenerator() = default;
    CByteReaderGenerator(const std::string & filepath);
    virtual ~CByteReaderGenerator();
};

} // namespace QCSP

#endif // _C_READ_GENERATOR_HPP_
