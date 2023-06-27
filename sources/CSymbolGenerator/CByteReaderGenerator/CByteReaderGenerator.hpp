
#ifndef _C_BYTE_READER_GENERATOR_HPP_
#define _C_BYTE_READER_GENERATOR_HPP_

#include "CSymbolGenerator/CSymbolGenerator.hpp"

#include <iostream>

namespace QCSP {

class CByteReaderGenerator : public CSymbolGenerator {
protected:
    std::istream *            byte_stream;
    static constexpr unsigned nb_bytes = (CSymbolGenerator::K * CSymbolGenerator::p) / 8U;
    std::vector<char>         buffer;

    virtual bool is_implemented() const = 0;

public:
    bool good() const { return this->byte_stream->good(); }
    bool eof() const { return this->byte_stream->eof(); }

    virtual void process(std::vector<int> & symbols) override;

    CByteReaderGenerator();
    CByteReaderGenerator(const CByteReaderGenerator &) = delete;
    CByteReaderGenerator(CByteReaderGenerator &&)      = delete;

    virtual ~CByteReaderGenerator() = default;
};

} // namespace QCSP

#endif // _C_BYTE_READER_GENERATOR_HPP_
