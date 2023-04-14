#include "./CCompleteModulator.hpp"

void QCSP::CCompleteModulator::encode(const std::vector<int> & message, std::vector<int> & codeword) {
}

void QCSP::CCompleteModulator::modulate(const std::vector<int> & codeword, std::vector<int> & ccsk_frame) {
}

void QCSP::CCompleteModulator::overmodulate(const std::vector<int> & ccsk_frame, std::vector<int> & frame) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < q; j++) {
            const int idx = i * q + j;
            frame[idx]    = ccsk_frame[idx] * om_sequence[i];
        }
    }
}

QCSP::CCompleteModulator::CCompleteModulator(
    int                      _N,
    int                      _K,
    int                      _p,
    const std::vector<int> & _pn,
    const std::vector<int> & _om)
    : N(_N),
      K(_K),
      p(_p),
      pn_sequence(_pn),
      om_sequence(_om) {
    // Initialize NB-LDPC Matrices
}
