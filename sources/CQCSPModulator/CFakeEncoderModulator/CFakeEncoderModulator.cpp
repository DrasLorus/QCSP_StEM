#include "CQCSPModulator/CFakeEncoderModulator/CFakeEncoderModulator.hpp"
#include "CQCSPModulator/NBLDPC_Matrices/nbldpc_matrices.hpp" // IWYU pragma: keep
#include "utilities/definitions.hpp"

QCSP::CFakeEncoderModulator::CFakeEncoderModulator(const std::vector<int> & _pn, const std::vector<int> & _om)
    : CCompleteModulator(_pn, _om) {
}

void QCSP::CFakeEncoderModulator::encode(const std::vector<uint_gf_t> &, std::vector<uint_gf_t> & codeword) {
    static_assert(CQCSPModulator::N == 60, "Only implemented for N == 60.");
    uint_gf_t symbolic_word[CQCSPModulator::N] = {21, 34, 12, 60, 60, 14, 58, 60, 57, 31, 33, 39, 43, 47, 60, 54, 7, 40, 22, 52, 2, 36, 18, 13, 14, 42, 47, 10, 56, 62, 44, 63, 43, 37, 16, 0, 12, 40, 4, 42, 63, 27, 30, 33, 51, 2, 33, 55, 20, 41, 30, 43, 30, 1, 59, 50, 36, 20, 13, 27};
    for (int n = 0; n < CQCSPModulator::N; n++) {
        codeword[n] = QCSP::GF_S2N[symbolic_word[n]];
    }
}
