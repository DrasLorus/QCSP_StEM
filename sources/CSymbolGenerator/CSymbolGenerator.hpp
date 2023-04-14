#ifndef _SYMBOL_GENERATOR_HPP_
#define _SYMBOL_GENERATOR_HPP_

#include <vector>

namespace QCSP {

class CSymbolGenerator {
public:
    virtual void process(std::vector<int> & symbols) = 0;
};

} // namespace QCSP

#endif // _SYMBOL_GENERATOR_HPP_
