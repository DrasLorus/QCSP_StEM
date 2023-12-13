/**
 * @file CFftwRootEngine.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _C_FFTW_ROOT_ENGINE_HPP_
#define _C_FFTW_ROOT_ENGINE_HPP_

#include <complex>
#include <cstddef>
#include <cstring>
#include <fftw3.h>
#include <type_traits>
#include <vector>

namespace QCSP {

/**
 * @brief Abstract class defining an Interface to encapsulate calls to FFTW3
 *
 * @tparam TFloat Overall floating point type
 * @tparam TPlan
 * @tparam TFFTW_in
 * @tparam TFFTW_out
 */
template <typename TFloat, typename TPlan, typename TFFTW_in, typename TFFTW_out>
class CFftwRootEngine {
    // TODO: Support other floating-point type ?
    // static_assert(std::is_floating_point<TFloat>::value, "TFloat have to be a floating-point type.");
    static_assert(std::is_same<TFloat, float>::value or std::is_same<TFloat, double>::value,
                  "The only supported floating-point types are 'float' and 'double'.");

protected:
    /// @brief FFTW plan type in use
    using plan_t = TPlan;
    /// @brief floating-point type in use
    using real_t = TFloat;
    /// @brief complex floating-point type in use, based on std::complex
    using complex_t = std::complex<real_t>;
    /// @brief FFTW vector type used in input
    using fftw_in_t = TFFTW_in;
    /// @brief FFTW vector type used in output
    using fftw_out_t = TFFTW_out;
    /// @brief complex floating-point vector in use, based on std::vector and complex_t
    using complex_vector_t = std::vector<complex_t>;

    /// @brief Counter of allocated FFTW engines
    static unsigned nb_allocated_engine;

public:
    /**
     * @brief Computes the FFT of input and places it in output
     *
     * @param input any complex_vector_t
     * @param output FFT of input
     */
    virtual void process(const complex_vector_t & input, complex_vector_t & output) = 0;

    CFftwRootEngine()          = default;
    virtual ~CFftwRootEngine() = default;
};

} // namespace QCSP

#endif // _C_FFTW_ROOT_ENGINE_HPP_