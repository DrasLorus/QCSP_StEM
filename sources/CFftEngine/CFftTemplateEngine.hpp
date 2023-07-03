#ifndef _C_FFTW_WRAPPER_HPP_
#define _C_FFTW_WRAPPER_HPP_

#include <complex>
#include <cstddef>
#include <cstring>
#include <fftw3.h>
#include <vector>

#include "./CFftwRootEngine.hpp"

namespace QCSP {

template <typename TFloat>
class CFftEngine : public CFftwRootEngine<TFloat> {
    using real_t         = TFloat;
    using complex_t      = std::complex<real_t>;
    using complex_vector = std::vector<complex_t>;

public:
    size_t       size() const;
    virtual void process(const complex_vector & input, complex_vector & output) override;

    CFftEngine(size_t size, unsigned plan_strategy);
    virtual ~CFftEngine();
};

template <typename TFloat>
class CIfftEngine : public CFftwRootEngine<TFloat> {
    using real_t         = TFloat;
    using complex_t      = std::complex<real_t>;
    using complex_vector = std::vector<complex_t>;

public:
    size_t       size() const;
    virtual void process(const complex_vector & input, complex_vector & output) override;

    CIfftEngine(size_t size, unsigned plan_strategy);
    virtual ~CIfftEngine();
};

} // namespace QCSP

#endif // _C_FFTW_WRAPPER_HPP_