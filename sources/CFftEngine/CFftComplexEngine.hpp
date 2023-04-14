#ifndef _FFT_COMPLEX_ENGINE_HPP_
#define _FFT_COMPLEX_ENGINE_HPP_

#include <complex>
#include <cstddef>
#include <cstring>
#include <fftw3.h>
#include <vector>

#include "./CFftTemplateEngine.hpp"

namespace QCSP {

template <>
class CFftEngine<std::complex<float>> {
    using real_t         = float;
    using complex_t      = std::complex<float>;
    using complex_vector = std::vector<complex_t>;

private:
    const size_t fft_size;

    fftwf_plan      plan_fft;
    fftwf_complex * ptr_in;
    fftwf_complex * ptr_out;

    const unsigned int fftw_plan_strat;

    void set_fftw();

    void clear_fftw();

public:
    size_t size() const;

    void process(const complex_vector & input, complex_vector & output);

    CFftEngine(size_t size, unsigned plan_strategy = FFTW_MEASURE);

    virtual ~CFftEngine();
};

// TFloat == double

template <>
class CFftEngine<std::complex<double>> {
    using real_t         = double;
    using complex_t      = std::complex<double>;
    using complex_vector = std::vector<complex_t>;

private:
    const size_t fft_size;

    fftw_plan      plan_fft;
    fftw_complex * ptr_in;
    fftw_complex * ptr_out;

    const unsigned int fftw_plan_strat;

    void set_fftw();
    void clear_fftw();

public:
    size_t size() const;

    void process(const complex_vector & input, complex_vector & output);

    CFftEngine(size_t size, unsigned plan_strategy = FFTW_MEASURE);
    virtual ~CFftEngine();
};

// TFloat == float

template <>
class CIfftEngine<std::complex<float>> {
    using real_t         = float;
    using complex_t      = std::complex<float>;
    using complex_vector = std::vector<complex_t>;

private:
    const size_t fft_size;

    fftwf_plan      plan_ifft;
    fftwf_complex * ptr_in;
    fftwf_complex * ptr_out;

    const unsigned int fftw_plan_strat;

    void set_fftw();
    void clear_fftw();

public:
    size_t size() const;

    void process(const complex_vector & input, complex_vector & output);

    CIfftEngine(size_t size, unsigned plan_strategy = FFTW_MEASURE);

    virtual ~CIfftEngine();
};

// TFloat == double

template <>
class CIfftEngine<std::complex<double>> {
    using real_t         = double;
    using complex_t      = std::complex<double>;
    using complex_vector = std::vector<complex_t>;

private:
    const size_t fft_size;

    fftw_plan      plan_ifft;
    fftw_complex * ptr_in;
    fftw_complex * ptr_out;

    const unsigned int fftw_plan_strat;

    void set_fftw();

    void clear_fftw();

public:
    size_t size() const;

    void process(const complex_vector & input, complex_vector & output);

    CIfftEngine(size_t size, unsigned plan_strategy = FFTW_MEASURE);

    virtual ~CIfftEngine();
};

template <class TFloat>
using CFftComplexEngine = CFftEngine<std::complex<TFloat>>;

template <class TFloat>
using CIfftComplexEngine = CIfftEngine<std::complex<TFloat>>;

} // namespace QCSP

#endif // _FFT_COMPLEX_ENGINE_HPP_