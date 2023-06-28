#ifndef _C_FAKE_ENCODER_MODULATOR_HPP_
#define _C_FAKE_ENCODER_MODULATOR_HPP_

#include <vector>

#include "CQCSPModulator/CCompleteModulator/CCompleteModulator.hpp"

namespace QCSP {

class CFakeEncoderModulator : public CCompleteModulator {
protected:
    virtual void encode(const std::vector<int> & message, std::vector<int> & codeword) override;

public:
    CFakeEncoderModulator(const std::vector<int> & _pn, const std::vector<int> & _om);
    virtual ~CFakeEncoderModulator() = default;
};

} // namespace QCSP

#endif // _C_FAKE_ENCODER_MODULATOR_HPP_
