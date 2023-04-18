/**
 * @file constants.hpp
 * @author Camille Monière
 * @brief NB-LDPC constants file for N60 K20 p6 (i.e., q64) DON'T INCLUDE DIRECTLY
 * @version 0.1
 * @date 2023-04-17
 *
 * @copyright Copyright (c) 2023
 *
 * @details This file defines NB-LDPC constants for N = 60, K = 20, and p = 6 (i.e., q = 64).
 * You shall not include this file directly. You have to use the 'configure_file' mechanism of cmake,
 * thus including the generated 'nbldpc_matrices.hpp' in the parent directory ('CQCSPModulator/NBLDPC_Matrices/nbldpc_matrices.hpp').
 */

#ifndef _QCSP_NBLDPC_CONSTANTS_HPP_
#define _QCSP_NBLDPC_CONSTANTS_HPP_

#include "utilities/definitions.hpp"

#define ENCODER_TYPE 0

namespace QCSP {

constexpr const unsigned int _MESSAGE_ = 120;

constexpr const unsigned int _DEG_CN_           = 3;
constexpr const unsigned int _DEG_COMPUTATIONS_ = 40;

constexpr const unsigned int _KBIT_ = (_KSYMBOL_ * _LOG2GF_);
constexpr const unsigned int _NBIT_ = (_NSYMBOL_ * _LOG2GF_);

constexpr const unsigned int _NmKSYMBOL_ = (_NSYMBOL_ - _KSYMBOL_);
constexpr const unsigned int _NmKBIT_    = (_NBIT_ - _KBIT_);

} // namespace QCSP

#endif // _QCSP_NBLDPC_CONSTANTS_HPP_
