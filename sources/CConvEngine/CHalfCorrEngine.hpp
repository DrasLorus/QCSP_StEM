#ifndef _HALF_CORR_ENGINE_HPP_
#define _HALF_CORR_ENGINE_HPP_

#include <typeinfo>

#include "CFftEngine/CFftComplexEngine.hpp"

namespace QCSP {

template <class TFloat>
class CHalfCorrEngine {
    static_assert(std::is_floating_point<TFloat>(), "CHalfCorrEngine TFloat have to be a floating-point scalar type.");
    using real_t         = TFloat;
    using complex_t      = std::complex<real_t>;
    using complex_vector = std::vector<complex_t>;

private:
    CFftComplexEngine<real_t>  fft_engine;
    CIfftComplexEngine<real_t> ifft_engine;

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
        static const real_t * ptr_op = reinterpret_cast<real_t *>(operand.data());
        static real_t *       ptr_i0 = reinterpret_cast<real_t *>(interm0.data());
        static real_t *       ptr_i1 = reinterpret_cast<real_t *>(interm1.data());

        fft_engine.process(input, interm0);

        for (int ri = 0; ri < (size() << 1); ri += 2) {
            const size_t ii = ri + 1;
            ptr_i1[ri]      = ptr_i0[ri] * ptr_op[ri] - ptr_i0[ii] * ptr_op[ii];
            ptr_i1[ii]      = ptr_i0[ri] * ptr_op[ii] + ptr_i0[ii] * ptr_op[ri];
        }

        ifft_engine.process(interm1, output);
    }
};

} // namespace QCSP

#endif // _CONV_ENGINE_HPP_
