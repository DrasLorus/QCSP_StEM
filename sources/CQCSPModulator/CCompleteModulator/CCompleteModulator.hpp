#ifndef _COMPLETE_MODULATOR_HPP_
#define _COMPLETE_MODULATOR_HPP_

#include <cmath>
#include <cstdint>
#include <vector>

#include "CQCSPModulator/CQCSPModulator.hpp"

namespace QCSP {

class CCompleteModulator : public CQCSPModulator {
private:
    const std::vector<int> pn_sequence;
    const std::vector<int> om_sequence;

protected:
    virtual void encode(const std::vector<int> & message, std::vector<int> & codeword);
    virtual void modulate(const std::vector<int> & codeword, std::vector<int> & ccsk_frame);
    virtual void overmodulate(const std::vector<int> & ccsk_frame, std::vector<int> & frame);

public:
    virtual void process(const std::vector<int> & input, std::vector<int> & output) override {
        static std::vector<int> codeword(CQCSPModulator::N, 0);
        static std::vector<int> ccsk_frame(CQCSPModulator::frame_size(), 0);
        encode(input, codeword);
        modulate(codeword, ccsk_frame);
        overmodulate(ccsk_frame, output);
    }

    CCompleteModulator(const std::vector<int> & _pn, const std::vector<int> & _om);
    virtual ~CCompleteModulator() = default;
};

} // namespace QCSP

#endif // _COMPLETE_MODULATOR_HPP_
