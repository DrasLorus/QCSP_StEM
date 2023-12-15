
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
    /// @brief a byte stream
    std::istream * byte_stream;
    /// @brief minimum number of bytes in a message
    static constexpr unsigned nb_bytes = (CSymbolGenerator::K * CSymbolGenerator::p) / 8U;
    /// @brief Internal byte buffer
    std::vector<char> buffer;

    /**
     * @brief Purely virtual method to require derivation
     *
     * @return true if byte_stream is usable
     * @return false if bytestream is not usable
     */
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

    /**
     * @brief Construct a new CByteReaderGenerator object
     *
     */
    CByteReaderGenerator();
    
    CByteReaderGenerator(const CByteReaderGenerator &) = delete;
    CByteReaderGenerator(CByteReaderGenerator &&)      = delete;

    virtual ~CByteReaderGenerator() = default;
};

} // namespace QCSP

#endif // _C_BYTE_READER_GENERATOR_HPP_
