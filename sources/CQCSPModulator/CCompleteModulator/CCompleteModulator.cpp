#include <algorithm>
#include <cassert>
#include <cstring>

#include "./CCompleteModulator.hpp"

#include "CQCSPModulator/NBLDPC_Matrices/nbldpc_matrices.hpp" // IWYU pragma: keep

void QCSP::CCompleteModulator::encode(const std::vector<int> & message, std::vector<int> & codeword) {
    const int * const KSYMB  = message.data();
    int *             CODEWD = codeword.data();

    static std::vector<int> NSYMBV(CQCSPModulator::N, 0);
    static int * const      NSYMB = NSYMBV.data();

    constexpr const int M = CQCSPModulator::N - CQCSPModulator::K;

    // std::cerr << K << " " << M << " " << N << " " << log2GF << std::endl;

#if (ENCODER_TYPE > 0)
    for (int k = 0; k < CQCSPModulator::K; k++) {
        NSYMB[M + k] = KSYMB[k];
    }

    for (int m = M - 1; m >= 0; m--) {
        int buf = 0;
        for (int n = m + 1; n < CQCSPModulator::N; n++) {
            const int alpha = QCSP::GF_ENC[m][n];
            if (alpha != 0) {
                const int mult = QCSP::GF_MUL[alpha][NSYMB[n]];
                buf            = QCSP::GF_ADD[buf][mult];
            }
        }
        assert(buf < CQCSPModulator::q);
        NSYMB[m] = QCSP::GF_DIV[buf][QCSP::GF_ENC[m][m]];
    }
#else
    std::memcpy(NSYMB, KSYMB, CQCSPModulator::K * sizeof(int));
    std::memset((NSYMB + CQCSPModulator::K), 0, M * sizeof(int));

    int cnt = 0;
    for (int j = K; j < CQCSPModulator::N; j++) {
        for (int i = 0; i < CQCSPModulator::K; i++) {
            const int alpha = QCSP::GF_ENC[cnt][i];
            if (alpha != 0) {
                const int mult = QCSP::GF_MUL[alpha][KSYMB[i]];
                NSYMB[j]       = QCSP::GF_ADD[NSYMB[j]][mult];
            }
        }
        cnt++;
        assert(NSYMB[j] < CQCSPModulator::q);
        // NSYMB[j] = buf;
    }

#endif

    for (int n = 0; n < CQCSPModulator::N; n++) {
        CODEWD[QCSP::GF_PERM[n]] = NSYMB[n];
    }

#if (defined(DEBUG) && (defined(SHUNT_ENCODER)))
    int8_t word_enc[60] = {21, 34, 12, 60, 60, 14, 58, 60, 57, 31, 33, 39, 43, 47, 60, 54, 7, 40, 22, 52, 2, 36, 18, 13, 14, 42, 47, 10, 56, 62, 44, 63, 43, 37, 16, 0, 12, 40, 4, 42, 63, 27, 30, 33, 51, 2, 33, 55, 20, 41, 30, 43, 30, 1, 59, 50, 36, 20, 13, 27};
    for (int n = 0; n < N; n++) {
        CODEWD[n] = word_enc[n];
    }
#endif
}

void QCSP::CCompleteModulator::modulate(const std::vector<int> & codeword, std::vector<int> & ccsk_frame) {
    const int * const src = codeword.data();
    int * const       dst = ccsk_frame.data();

    const int * const p_pn_begin = pn_sequence.data();
    const int * const p_pn_end   = pn_sequence.data() + CQCSPModulator::pn_size();

    for (int sb_idx = 0; sb_idx < CQCSPModulator::codeword_size(); sb_idx++) {
        int * const       curr_symbol = dst + sb_idx * CQCSPModulator::pn_size();
        const int * const p_middle    = p_pn_begin + src[sb_idx];

        // PN Rotation
        // do: [begin, ···, middle, ···, end] → [middle, ···, end, begin, ···, middle - 1 ]
        std::rotate_copy(p_pn_begin, p_middle, p_pn_end, curr_symbol);
    }
}

void QCSP::CCompleteModulator::overmodulate(const std::vector<int> & ccsk_frame, std::vector<int> & frame) {
    for (int i = 0; i < CQCSPModulator::N; i++) {
        for (int j = 0; j < CQCSPModulator::q; j++) {
            const int idx = i * CQCSPModulator::q + j;
            frame[idx]    = ccsk_frame[idx] * this->om_sequence[i];
        }
    }
}

QCSP::CCompleteModulator::CCompleteModulator(
    const std::vector<int> & _pn,
    const std::vector<int> & _om)
    : pn_sequence(_pn),
      om_sequence(_om) {
}
