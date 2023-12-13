#ifndef _C_FFTW_ROOT_ENGINE_HPP_
#define _C_FFTW_ROOT_ENGINE_HPP_

#include <complex>
#include <cstddef>
#include <cstring>
#include <fftw3.h>
#include <vector>

namespace QCSP {

/**
 * @brief Abstract class defining an Interface to encapsulate calls to FFTW3
 * 
 * @tparam TFloat FLoating-point type 
 */
template <typename TFloat>
class CFftwRootEngine {
protected:
    using real_t         = TFloat;
    using complex_t      = std::complex<real_t>;
    using complex_vector = std::vector<complex_t>;

    static unsigned nb_allocated_engine;

public:
    virtual void process(const complex_vector & input, complex_vector & output) = 0;

    CFftwRootEngine()          = default;
    virtual ~CFftwRootEngine() = default;
};

} // namespace QCSP

#endif // _C_FFTW_ROOT_ENGINE_HPP_