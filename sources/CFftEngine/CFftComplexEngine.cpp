#include <complex>
#include <cstddef>
#include <cstring>
#include <fftw3.h>
#include <vector>

#include "./CFftComplexEngine.hpp"
#include "./CFftTemplateEngine.hpp"

/******* FFT ANY FLOAT ********/

template <>
unsigned QCSP::CFftwRootEngine<float>::nb_allocated_engine = 0;

template <>
unsigned QCSP::CFftwRootEngine<double>::nb_allocated_engine = 0;

/******* FFT COMPLEX FLOAT ********/

size_t QCSP::CFftEngine<std::complex<float>>::size() const { return fft_size; }

void QCSP::CFftEngine<std::complex<float>>::set_fftw() {
    // TODO : Implement wisdom usage
    // if (base::nb_allocated_engine++ == 0) {
    //     fftwf_import_wisdom_from_filename("wisdom.txt");
    // }
    base::nb_allocated_engine++;

    ptr_in  = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * fft_size);
    ptr_out = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * fft_size);

    plan_fft = fftwf_plan_dft_1d(fft_size, ptr_in, ptr_out, FFTW_FORWARD, fftw_plan_strat);
    if (plan_fft == nullptr) {
        throw std::runtime_error("FFT planification failed.");
    }
}

void QCSP::CFftEngine<std::complex<float>>::clear_fftw() {
    fftwf_free(ptr_in);
    fftwf_free(ptr_out);
    fftwf_destroy_plan(plan_fft);

    if (--base::nb_allocated_engine == 0) {
        fftwf_cleanup();
    }
}

void QCSP::CFftEngine<std::complex<float>>::process(const complex_vector & input, complex_vector & output) {
    const real_t * in = (const real_t *) input.data();
    std::memcpy((real_t *) ptr_in, in, fft_size * 2 * sizeof(real_t));

    fftwf_execute(plan_fft);

    real_t * out = (real_t *) output.data();
    std::memcpy((real_t *) out, (real_t *) ptr_out, fft_size * 2 * sizeof(real_t));
}

QCSP::CFftEngine<std::complex<float>>::CFftEngine(size_t size, unsigned plan_strategy)
    : fft_size(size),
      fftw_plan_strat(plan_strategy) {
    set_fftw();
}

QCSP::CFftEngine<std::complex<float>>::~CFftEngine() { clear_fftw(); }

/******* IFFT COMPLEX FLOAT ********/

void QCSP::CIfftEngine<std::complex<float>>::set_fftw() {
    // TODO : Implement wisdom usage
    // if (base::nb_allocated_engine++ == 0) {
    //     fftwf_import_wisdom_from_filename("wisdom.txt");
    // }
    base::nb_allocated_engine++;

    ptr_in  = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * fft_size);
    ptr_out = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * fft_size);

    plan_ifft = fftwf_plan_dft_1d(fft_size, ptr_in, ptr_out, FFTW_BACKWARD, fftw_plan_strat);
    if (plan_ifft == nullptr) {
        throw std::runtime_error("IFFT planification failed.");
    }
}

void QCSP::CIfftEngine<std::complex<float>>::clear_fftw() {
    fftwf_free(ptr_in);
    fftwf_free(ptr_out);
    fftwf_destroy_plan(plan_ifft);

    if (--base::nb_allocated_engine == 0) {
        fftwf_cleanup();
    }
}

size_t QCSP::CIfftEngine<std::complex<float>>::size() const { return fft_size; }

void QCSP::CIfftEngine<std::complex<float>>::process(const complex_vector & input, complex_vector & output) {
    const real_t * in = (const real_t *) input.data();
    std::memcpy((real_t *) ptr_in, in, fft_size * 2 * sizeof(real_t));

    fftwf_execute(plan_ifft);

    real_t * out = (real_t *) output.data();

    for (int i = 0; i < fft_size * 2; i++) {
        out[i] = ((real_t *) ptr_out)[i];
    }
}

QCSP::CIfftEngine<std::complex<float>>::CIfftEngine(size_t size, unsigned plan_strategy)
    : fft_size(size),
      fftw_plan_strat(plan_strategy) {
    set_fftw();
}

QCSP::CIfftEngine<std::complex<float>>::~CIfftEngine() { clear_fftw(); }

/******* IFFT COMPLEX DOUBLE *******/

void QCSP::CFftEngine<std::complex<double>>::set_fftw() {
    // TODO : Implement wisdom usage
    // if (base::nb_allocated_engine++ == 0) {
    //     fftw_import_wisdom_from_filename("wisdom.txt");
    // }
    base::nb_allocated_engine++;

    ptr_in  = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * fft_size);
    ptr_out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * fft_size);

    plan_fft = fftw_plan_dft_1d(fft_size, ptr_in, ptr_out, FFTW_FORWARD, fftw_plan_strat);
    if (plan_fft == nullptr) {
        throw std::runtime_error("FFT planification failed.");
    }
}

void QCSP::CFftEngine<std::complex<double>>::clear_fftw() {
    fftw_free(ptr_in);
    fftw_free(ptr_out);
    fftw_destroy_plan(plan_fft);

    if (--base::nb_allocated_engine == 0) {
        fftw_cleanup();
    }
}

size_t QCSP::CFftEngine<std::complex<double>>::size() const { return fft_size; }

void QCSP::CFftEngine<std::complex<double>>::process(const complex_vector & input, complex_vector & output) {
    const real_t * in = (const real_t *) input.data();
    std::memcpy((real_t *) ptr_in, in, fft_size * 2 * sizeof(real_t));

    fftw_execute(plan_fft);

    real_t * out = (real_t *) output.data();
    std::memcpy((real_t *) out, (real_t *) ptr_out, fft_size * 2 * sizeof(real_t));
}

QCSP::CFftEngine<std::complex<double>>::CFftEngine(size_t size, unsigned plan_strategy)
    : fft_size(size),
      fftw_plan_strat(plan_strategy) {
    set_fftw();
}

QCSP::CFftEngine<std::complex<double>>::~CFftEngine() { clear_fftw(); }

void QCSP::CIfftEngine<std::complex<double>>::set_fftw() {
    // TODO : Implement wisdom usage
    // if (base::nb_allocated_engine++ == 0) {
    //     fftw_import_wisdom_from_filename("wisdom.txt");
    // }
    base::nb_allocated_engine++;

    ptr_in  = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * fft_size);
    ptr_out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * fft_size);

    plan_ifft = fftw_plan_dft_1d(fft_size, ptr_in, ptr_out, FFTW_BACKWARD, fftw_plan_strat);
    if (plan_ifft == nullptr) {
        throw std::runtime_error("IFFT planification failed.");
    }
}

void QCSP::CIfftEngine<std::complex<double>>::clear_fftw() {
    fftw_free(ptr_in);
    fftw_free(ptr_out);
    fftw_destroy_plan(plan_ifft);

    if (--base::nb_allocated_engine == 0) {
        fftw_cleanup();
    }
}

size_t QCSP::CIfftEngine<std::complex<double>>::size() const { return fft_size; }

void QCSP::CIfftEngine<std::complex<double>>::process(const complex_vector & input, complex_vector & output) {
    const real_t * in = (const real_t *) input.data();
    std::memcpy((real_t *) ptr_in, in, fft_size * 2 * sizeof(real_t));

    fftw_execute(plan_ifft);

    real_t * out = (real_t *) output.data();
    for (int i = 0; i < fft_size * 2; i++) {
        out[i] = ((real_t *) ptr_out)[i];
    }
}

QCSP::CIfftEngine<std::complex<double>>::CIfftEngine(size_t size, unsigned plan_strategy)
    : fft_size(size),
      fftw_plan_strat(plan_strategy) {
    set_fftw();
}

QCSP::CIfftEngine<std::complex<double>>::~CIfftEngine() { clear_fftw(); }
