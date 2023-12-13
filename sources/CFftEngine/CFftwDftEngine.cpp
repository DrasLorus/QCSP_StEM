#include <complex>
#include <cstddef>
#include <cstring>
#include <fftw3.h>
#include <vector>

#include "./CFftwDftEngine.hpp"

/******* FFT ANY FLOAT ********/

template <>
unsigned QCSP::CFftwDftEngineInterface<float>::nb_allocated_engine = 0;

template <>
unsigned QCSP::CFftwDftEngineInterface<double>::nb_allocated_engine = 0;
