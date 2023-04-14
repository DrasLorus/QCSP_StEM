#ifndef _QCSP_MODULATOR_HPP_
#define _QCSP_MODULATOR_HPP_

#include <vector>

namespace QCSP {

class CQCSPModulator {
public:
    virtual void process(const std::vector<int> & input, std::vector<int> & output) = 0;
};

} // namespace QCSP

#endif // _QCSP_MODULATOR_HPP_
