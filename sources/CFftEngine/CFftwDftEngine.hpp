/**
 * @file CFftwDftEngine.hpp
 * @author Camille Monière (camille.moniere@univ-ubs.fr)
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

#include "CFftEngine/CFftwRootEngine.hpp"

namespace QCSP {

/**
 * @brief A wrapper around CFftwRootEngine suited for ND-complex DFT
 *
 * @tparam TFloat targeted floating-point type (either 'float' or 'double')
 */
template <typename TFloat>
using CFftwDftEngineInterface = CFftwRootEngine<
    TFloat,
    std::conditional_t<std::is_same<double, TFloat>::value, fftw_plan, fftwf_plan>,
    std::conditional_t<std::is_same<double, TFloat>::value, fftw_complex, fftwf_complex>,
    std::conditional_t<std::is_same<double, TFloat>::value, fftw_complex, fftwf_complex>>;

/**
 * @brief a generic class encapsulating FFTW3 API to perform D-complex DFT, with memory handled
 *
 * @details The class encapsulates and generalizes calls to FFTW3 functions involved
 * in performing 1D complex floating-point DFTs. Memory allocations and clean-ups are
 * automatically performed.
 * See http://www.fftw.org/fftw3_doc/Complex-One_002dDimensional-DFTs.html
 *
 * @tparam TFloat target floating-point (32-bit float or 64-bit double)
 */
template <typename TFloat, bool TForward = true>
class CFftwDftEngine : public CFftwDftEngineInterface<TFloat> {
public:
    /// @brief Base class
    using Base = CFftwDftEngineInterface<TFloat>;

    static_assert(std::is_same<typename Base::fftw_in_t, typename Base::fftw_out_t>::value,
                  "Implementation error.");

    using typename Base::complex_t;
    using typename Base::complex_vector_t;
    using typename Base::plan_t;
    using typename Base::real_t;

protected:
    /// @brief internal fftw complex vector
    using fftw_complex_t = typename Base::fftw_in_t;

    /// @brief Perform FFT when TForward is true, and IFFT otherwise.
    static constexpr int FFTW_TYPE = TForward ? FFTW_FORWARD : FFTW_BACKWARD;

    /**
     * @brief interface to fftw[f]_execute
     *
     * @param p a plan
     */
    static void fftw_execute(const plan_t p) {
        if constexpr (std::is_same<plan_t, fftwf_plan>::value) {
            ::fftwf_execute(p);
        } else if constexpr (std::is_same<plan_t, fftw_plan>::value) {
            ::fftw_execute(p);
        }
    }

    /**
     * @brief interface to fftw[f]_malloc
     *
     * @param n requested memory in bytes
     */
    static void * fftw_malloc(size_t n) {
        if constexpr (std::is_same<TFloat, float>::value) {
            return ::fftwf_malloc(n);
        } else {
            return ::fftw_malloc(n);
        }
    }

    /**
     * @brief interface to fftw[f]_plan_dft_1d, see http://www.fftw.org/fftw3_doc/Complex-DFTs.html
     *
     * @param n underlying FFT size
     * @param in input vector
     * @param out output vector
     * @param sign forward or backward
     * @param flags additional planning flags, see http://www.fftw.org/fftw3_doc/Planner-Flags.html
     * @return plan_t a FFTW3 plan, or nullptr in case of error
     */
    static plan_t fftw_plan_dft_1d(int n, fftw_complex_t * in, fftw_complex_t * out, int sign, unsigned int flags) {
        if constexpr (std::is_same<fftw_complex_t, fftwf_complex>::value) {
            return ::fftwf_plan_dft_1d(n, in, out, sign, flags);
        } else if constexpr (std::is_same<fftw_complex_t, fftw_complex>::value) {
            return ::fftw_plan_dft_1d(n, in, out, sign, flags);
        }
    }

    /**
     * @brief interface to fftw[f]_free
     *
     * @param p pointer to an array of fftw_complex_t
     */
    static void fftw_free(void * p) {
        if constexpr (std::is_same<TFloat, float>::value) {
            ::fftwf_free(p);
        } else {
            ::fftw_free(p);
        }
    }

    /**
     * @brief interface to fftw[f]_destroy_plan
     *
     * @param p a plan allocated with fftw_plan_dft_1d()
     */
    static void fftw_destroy_plan(const plan_t p) {
        if constexpr (std::is_same<plan_t, fftwf_plan>::value) {
            ::fftwf_destroy_plan(p);
        } else if constexpr (std::is_same<plan_t, fftw_plan>::value) {
            ::fftw_destroy_plan(p);
        }
    }

    /**
     * @brief interface to fftw[f]_cleanup, called when all FFTW3 engines are destructed
     *
     */
    static void fftw_cleanup() {
        if constexpr (std::is_same<TFloat, float>::value) {
            ::fftw_cleanup();
        } else {
            ::fftwf_cleanup();
        }
    }

private:
    /// @brief actual size of the FFTW3 engine
    const size_t fft_size;

    /// @brief plan used by the FFTW3 engine
    plan_t plan_fft;
    /// @brief internal FFTW3-allocated input of the FFTW3 engine
    fftw_complex_t * ptr_in;
    /// @brief internal FFTW3-allocated output of the FFTW3 engine
    fftw_complex_t * ptr_out;

    /// @brief FFTW3 plan strategy in use, see http://www.fftw.org/fftw3_doc/Planner-Flags.html
    const unsigned int fftw_plan_strat;

    /**
     * @brief Safely setup the FFTW3 engine
     *
     * @details The setup includes allocating internal input and output, defining the plan
     * and incrementing the allocated engine counter.
     *
     */
    void set_fftw() {
        /// TODO : Implement wisdom usage
        // if (Base::nb_allocated_engine++ == 0) {
        //     fftwf_import_wisdom_from_filename("wisdom.txt");
        // }
        Base::nb_allocated_engine++;

        ptr_in  = (fftw_complex_t *) fftw_malloc(sizeof(fftw_complex_t) * fft_size);
        ptr_out = (fftw_complex_t *) fftw_malloc(sizeof(fftw_complex_t) * fft_size);

        plan_fft = fftw_plan_dft_1d(fft_size, ptr_in, ptr_out, FFTW_TYPE, fftw_plan_strat);
        if (plan_fft == nullptr) {
            throw std::runtime_error("FFTW planification failed.");
        }
    }

    /**
     * @brief Safely destroy the underlying FFTW3 engine
     *
     * @details This includes deallocating internal input and output, destroying the plan,
     * decrementing the allocated engine counter, and if it reaches 0, calling fftw_cleanup().
     *
     */
    void clear_fftw() {
        fftw_free(ptr_in);
        fftw_free(ptr_out);
        fftw_destroy_plan(plan_fft);

        if (--Base::nb_allocated_engine == 0) {
            fftw_cleanup();
        }
    }

public:
    /**
     * @brief get the size of the underlying FFT
     *
     * @return size_t the size
     */
    size_t size() const { return fft_size; }

    /**
     * @brief Safely compute the FFT using FFTW3
     *
     * @details input is copied into the internal FFTW3-allocated input ptr_in, the plan plan_fft is executed with fftw_execute(),
     * and the internal FFTW3-allocated output ptr_out is copied to output. While this involves two copies,
     * it ensures that the result is well-formed and correct.
     *
     * @param input any valid complex_vector_t of size size()
     * @param output any complex_vector_t, will contain the FFT of input
     */
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
     * @param size length of the underlying FFT
     * @param plan_strategy use during FFTW planning phase
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
