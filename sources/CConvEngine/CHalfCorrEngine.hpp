/**
 * @file CHalfCorrEngine.hpp
 * @author Camille Monière (camille.moniere@univ-ubs.fr)
 * @brief
 * @version 0.1
 * @date 2023-12-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef _HALF_CORR_ENGINE_HPP_
#define _HALF_CORR_ENGINE_HPP_

#include <typeinfo>

#include "CFftEngine/CFftComplexEngine.hpp"

namespace QCSP {

/**
 * @brief Define a half-sized correlation engine based on FFTW3
 *
 * @tparam TFloat floating-point type
 */
template <class TFloat>
class CHalfCorrEngine {
    static_assert(std::is_floating_point<TFloat>(), "CHalfCorrEngine TFloat have to be a floating-point scalar type.");

public:
    /// @brief FFT Engine type
    using fft_engine_t = CFftComplexEngine<TFloat>;
    /// @brief IFFT Engine type
    using ifft_engine_t = CIfftComplexEngine<TFloat>;
    /// @brief Complex type
    using complex_t = typename fft_engine_t::complex_t;
    /// @brief Complex vector type
    using complex_vector_t = typename fft_engine_t::complex_vector_t;
    /// @brief Real type
    using real_t = typename fft_engine_t::real_t;

private:
    /// @brief FFT Engine
    fft_engine_t fft_engine;
    /// @brief IFFT Engine
    ifft_engine_t ifft_engine;

    /// @brief internal intermediate vector - FFT of input
    complex_vector_t interm0;
    /// @brief internal intermediate vector - FFT products
    complex_vector_t interm1;
    /// @brief internal intermediate vector - FFT of time_operand used in CHalfCorrEngine()
    complex_vector_t operand;

public:
    /// @brief size of the underlying [I]FFT
    size_t size() const { return ifft_engine.size(); }

    /**
     * @brief Construct a new CHalfCorrEngine object
     *
     * @param size required size of the underlying [I]FFT
     * @param time_operand filter of constant to correlate with
     * @param plan_strategy FFTW plan used in [I]FFTs, see CFftComplexEngine
     */
    CHalfCorrEngine(size_t size, const complex_vector_t & time_operand, unsigned int plan_strategy = FFTW_MEASURE)
        : fft_engine(size, plan_strategy),
          ifft_engine(size, plan_strategy),
          interm0(size, 0),
          interm1(size, 0),
          operand(size, 0) {
        fft_engine.process(time_operand, operand);
    }

    /**
     * @brief Correlate the input with IFFT( operand ) and places the result in output
     *
     * @param input a complex_vector_t of size size()
     * @param output a complex_vector_t of size size()
     */
    void process(const complex_vector_t & input, complex_vector_t & output) {
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
