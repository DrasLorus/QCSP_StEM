#ifndef _HALF_CORR_ENGINE_HPP_
#define _HALF_CORR_ENGINE_HPP_

#include "CFftEngine/CFftComplexEngine.hpp"

namespace QCSP {

template <class TFloat>
class CHalfCorrEngine {
    using real_t         = TFloat;
    using complex_t      = std::complex<real_t>;
    using complex_vector = std::vector<complex_t>;

private:
    CFftEngine<complex_t>  fft_engine;
    CIfftEngine<complex_t> ifft_engine;

    complex_vector interm0;
    complex_vector interm1;
    complex_vector operand;

public:
    size_t size() const { return ifft_engine.size(); }

    CHalfCorrEngine(size_t size, const complex_vector & time_operand, unsigned int plan_strategy = FFTW_MEASURE)
        : fft_engine(size, plan_strategy),
          ifft_engine(size, plan_strategy),
          interm0(size, 0),
          interm1(size, 0),
          operand(size, 0) {
        fft_engine.process(time_operand, operand);
    }

    void process(const complex_vector & input, complex_vector & output) {
        fft_engine.process(input, interm0);

        for (int i = 0; i < size(); i++) {
            interm1[i] = interm0[i] * operand[i];
        }

        ifft_engine.process(interm1, output);
    }
};

} // namespace QCSP

#endif // _CONV_ENGINE_HPP_
