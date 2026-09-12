/**
 * @file CFftComplexEngine.hpp
 * @author Camille Monière (camille.moniere@univ-ubs.fr)
 * @brief
 * @version 0.1
 * @date 2023-12-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef _FFT_COMPLEX_ENGINE_HPP_
#define _FFT_COMPLEX_ENGINE_HPP_

#include <complex>
#include <cstddef>
#include <cstring>
#include <fftw3.h>
#include <vector>

#include "CFftEngine/CFftwDftEngine.hpp"

namespace QCSP {

/**
 * @brief Alias class for Complex FFT
 *
 * @tparam TFloat
 */
template <class TFloat>
using CFftComplexEngine = CFftwDftEngine<TFloat, true>;

/**
 * @brief Alias class for Complex IFFT
 *
 * @tparam TFloat
 */
template <class TFloat>
using CIfftComplexEngine = CFftwDftEngine<TFloat, false>;

} // namespace QCSP

#endif // _FFT_COMPLEX_ENGINE_HPP_