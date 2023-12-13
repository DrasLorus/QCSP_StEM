/**
 * @file CFftwDftEngine.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-12-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _C_FFTW_DFT_ENGINE_HPP_
#define _C_FFTW_DFT_ENGINE_HPP_

#include <complex>
#include <cstddef>
#include <cstring>
#include <fftw3.h>
#include <type_traits>
#include <vector>

#include "./CFftwRootEngine.hpp"

namespace QCSP {

template <typename TFloat>
using CFftwDftEngineInterface = CFftwRootEngine<
    TFloat,
    std::conditional_t<std::is_same<double, TFloat>::value, fftw_plan, fftwf_plan>,
    std::conditional_t<std::is_same<double, TFloat>::value, fftw_complex, fftwf_complex>,
    std::conditional_t<std::is_same<double, TFloat>::value, fftw_complex, fftwf_complex>>;

template <typename TFloat, bool TForward = true>
class CFftwDftEngine : public CFftwDftEngineInterface<TFloat> {
public:
    /// @brief Base class
    using base = CFftwDftEngineInterface<TFloat>;

protected:
    static_assert(std::is_same<typename base::fftw_in_t, typename base::fftw_out_t>::value,
                  "Implementation error.");

    using typename base::complex_t;
    using typename base::complex_vector_t;
    using typename base::plan_t;
    using typename base::real_t;

    /// @brief internal fftw complex vector
    using fftw_complex_t = typename base::fftw_in_t;

    /// @brief Perform FFT when TForward is true, and IFFT otherwise.
    static constexpr int FFTW_TYPE = TForward ? FFTW_FORWARD : FFTW_BACKWARD;

    static void fftw_execute(const fftwf_plan p) {
        ::fftwf_execute(p);
    }

    static void fftw_execute(const fftw_plan p) {
        ::fftw_execute(p);
    }

    static void * fftw_malloc(size_t n) {
        if constexpr (std::is_same<TFloat, float>::value) {
            return ::fftwf_malloc(n);
        } else {
            return ::fftw_malloc(n);
        }
    }

    static fftwf_plan fftw_plan_dft_1d(int n, fftwf_complex * in, fftwf_complex * out, int sign, unsigned int flags) {
        return ::fftwf_plan_dft_1d(n, in, out, sign, flags);
    }

    static fftw_plan fftw_plan_dft_1d(int n, fftw_complex * in, fftw_complex * out, int sign, unsigned int flags) {
        return ::fftw_plan_dft_1d(n, in, out, sign, flags);
    }

    static void fftw_free(void * p) {
        if constexpr (std::is_same<TFloat, float>::value) {
            ::fftwf_free(p);
        } else {
            ::fftw_free(p);
        }
    }

    static void fftw_destroy_plan(const fftw_plan p) {
        ::fftw_destroy_plan(p);
    }

    static void fftw_destroy_plan(const fftwf_plan p) {
        ::fftwf_destroy_plan(p);
    }

    static void fftw_cleanup() {
        if constexpr (std::is_same<TFloat, float>::value) {
            ::fftw_cleanup();
        } else {
            ::fftwf_cleanup();
        }
    }

private:
    const size_t fft_size;

    plan_t           plan_fft;
    fftw_complex_t * ptr_in;
    fftw_complex_t * ptr_out;

    const unsigned int fftw_plan_strat;

    void set_fftw() {
        // TODO : Implement wisdom usage
        // if (base::nb_allocated_engine++ == 0) {
        //     fftwf_import_wisdom_from_filename("wisdom.txt");
        // }
        base::nb_allocated_engine++;

        ptr_in  = (fftw_complex_t *) fftw_malloc(sizeof(fftw_complex_t) * fft_size);
        ptr_out = (fftw_complex_t *) fftw_malloc(sizeof(fftw_complex_t) * fft_size);

        plan_fft = fftw_plan_dft_1d(fft_size, ptr_in, ptr_out, FFTW_TYPE, fftw_plan_strat);
        if (plan_fft == nullptr) {
            throw std::runtime_error("FFTW planification failed.");
        }
    }

    void clear_fftw() {
        fftw_free(ptr_in);
        fftw_free(ptr_out);
        fftw_destroy_plan(plan_fft);

        if (--base::nb_allocated_engine == 0) {
            fftw_cleanup();
        }
    }

public:
    size_t size() const { return fft_size; }

    virtual void process(const complex_vector_t & input, complex_vector_t & output) override {
        const real_t * in = (const real_t *) input.data();
        std::memcpy((real_t *) ptr_in, in, fft_size * 2 * sizeof(real_t));

        fftw_execute(plan_fft);

        real_t * out = (real_t *) output.data();
        std::memcpy((real_t *) out, (real_t *) ptr_out, fft_size * 2 * sizeof(real_t));
    }

    /**
     * @brief Construct a new CFftwDftEngine object
     *
     * @param size
     * @param plan_strategy
     */
    CFftwDftEngine(size_t size, unsigned plan_strategy = FFTW_MEASURE)
        : fft_size(size),
          fftw_plan_strat(plan_strategy) {
        set_fftw();
    }

    /**
     * @brief Destroy the CFftwDftEngine object
     *
     */
    virtual ~CFftwDftEngine() { clear_fftw(); }
};

} // namespace QCSP

#endif // _C_FFTW_DFT_ENGINE_HPP_