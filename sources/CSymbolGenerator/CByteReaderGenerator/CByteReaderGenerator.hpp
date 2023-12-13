
#ifndef _C_BYTE_READER_GENERATOR_HPP_
#define _C_BYTE_READER_GENERATOR_HPP_

#include "CSymbolGenerator/CSymbolGenerator.hpp"

#include <iostream>

namespace QCSP {

/**
 * @brief Abstract class defining a high level interface to handle byte streams
 *
 */
class CByteReaderGenerator : public CSymbolGenerator {
protected:
    std::istream *            byte_stream;
    static constexpr unsigned nb_bytes = (CSymbolGenerator::K * CSymbolGenerator::p) / 8U;
    std::vector<char>         buffer;

    virtual bool is_implemented() const = 0;

public:
    /**
     * @brief return the state of the underlying stream
     *
     * @return true
     * @return false
     */
    bool good() const { return this->byte_stream->good(); }

    /**
     * @brief return the EOF state of the underlying stream
     *
     * @return true
     * @return false
     */
    bool eof() const { return this->byte_stream->eof(); }

    virtual void process(std::vector<int> & symbols) override;

    CByteReaderGenerator();
    CByteReaderGenerator(const CByteReaderGenerator &) = delete;
    CByteReaderGenerator(CByteReaderGenerator &&)      = delete;

    virtual ~CByteReaderGenerator() = default;
};

} // namespace QCSP

#endif // _C_BYTE_READER_GENERATOR_HPP_
