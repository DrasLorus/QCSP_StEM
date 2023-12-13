#ifndef _FFT_COMPLEX_ENGINE_HPP_
#define _FFT_COMPLEX_ENGINE_HPP_

#include <complex>
#include <cstddef>
#include <cstring>
#include <fftw3.h>
#include <vector>

#include "./CFftwDftEngine.hpp"

namespace QCSP {

template <class TFloat>
using CFftComplexEngine = CFftwDftEngine<TFloat, true>;

template <class TFloat>
using CIfftComplexEngine = CFftwDftEngine<TFloat, false>;

} // namespace QCSP

#endif // _FFT_COMPLEX_ENGINE_HPP_