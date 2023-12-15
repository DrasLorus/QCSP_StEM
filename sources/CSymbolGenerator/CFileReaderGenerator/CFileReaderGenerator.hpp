
#ifndef _C_FILE_READER_GENERATOR_HPP_
#define _C_FILE_READER_GENERATOR_HPP_

#include "CSymbolGenerator/CByteReaderGenerator/CByteReaderGenerator.hpp"

#include <fstream>
#include <iostream>

namespace QCSP {

/**
 * @brief Implement a CByteReaderGenerator specialized in reading file stream
 * 
 */
class CFileReaderGenerator : public CByteReaderGenerator {
protected:
    std::ifstream * file_stream() const { return static_cast<std::ifstream *>(byte_stream); }

    virtual bool is_implemented() const final { return true; };

public:
    bool is_open() const { return this->file_stream()->is_open(); }
    void open(const std::string & filepath);
    void close();

    CFileReaderGenerator();
    CFileReaderGenerator(const std::string & filepath);
    virtual ~CFileReaderGenerator();
};

} // namespace QCSP

#endif // _C_FILE_READER_GENERATOR_HPP_
