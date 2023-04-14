#ifndef _COMPLETE_MODULATOR_HPP_
#define _COMPLETE_MODULATOR_HPP_

#include <cmath>
#include <cstdint>
#include <vector>

#include "CQCSPModulator/CQCSPModulator.hpp"

namespace QCSP {

class CCompleteModulator : CQCSPModulator {
private:
    const uint32_t N;
    const uint32_t K;
    const uint32_t p;

    const uint32_t q          = 1 << p;
    const uint32_t frame_size = N * q;

    const std::vector<int> pn_sequence;
    const std::vector<int> om_sequence;

    // NB-LDPC Matrices

    void encode(const std::vector<int> & message, std::vector<int> & codeword);
    void modulate(const std::vector<int> & codeword, std::vector<int> & ccsk_frame);
    void overmodulate(const std::vector<int> & ccsk_frame, std::vector<int> & frame);

public:
    virtual void process(const std::vector<int> & input, std::vector<int> & output) override {
        static std::vector<int> codeword(N, 0);
        static std::vector<int> ccsk_frame(N, 0);
        encode(input, codeword);
        modulate(codeword, ccsk_frame);
        overmodulate(ccsk_frame, output);
    }

    CCompleteModulator(int _N, int _K, int _p, const std::vector<int> & _pn, const std::vector<int> & _om);
    virtual ~CCompleteModulator() = default;
};

} // namespace QCSP

#endif // _COMPLETE_MODULATOR_HPP_
