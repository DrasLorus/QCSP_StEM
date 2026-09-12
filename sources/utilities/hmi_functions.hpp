/**
 * @file hmi_functions.hpp
 * @author Camille MONIÈRE (camille.moniere@univ-ubs.fr)
 * @brief
 * @version 1.0
 * @date 2023-12-13
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef _HMI_FUNCTIONS_HPP_
#define _HMI_FUNCTIONS_HPP_

#include <vector>

#include <boost/program_options/variables_map.hpp>

#include "utilities/structures.hpp"

#include <iostream>

namespace QCSP {

/**
 * @brief Helper stream for warning log messages
 *
 * @param os the targeted ostream (default: std::cerr)
 * @return std::ostream& os
 */
std::ostream & warning_stream(std::ostream & os = std::cerr);

/**
 * @brief Helper function used for alignement
 *
 * @return const char* const space filler
 */
const char * const line_filler();

/**
 * @brief Helper function parsing argc et argv into vm
 *
 * @param argc number of program arguments
 * @param argv argument array variable
 * @param vm variable map
 * @return int EXIT_SUCCESS on success, EXIT_FAILURE otherwise
 */
int parse_user_input(int argc, char * argv[], boost::program_options::variables_map & vm);

/**
 * @brief Parse the variable map into an emitter_parameters structure
 *
 * @param vm Variable Map as produced by parse_user_input
 * @param prm An emitter_parameters which can be used to initialized a transmitter
 */
void parse_vm(const boost::program_options::variables_map & vm, emitter_parameters & prm);

/**
 * @brief
 *
 * @param filename
 * @param n_frame
 * @param n_s
 * @param pn
 * @param best_N
 * @param h_filter
 */
void load_settings(
    const std::string &                filename,
    unsigned &                         n_frame,
    unsigned &                         n_s,
    std::vector<int> &                 pn,
    std::vector<int> &                 best_N,
    std::vector<std::complex<float>> & h_filter);

} // namespace QCSP

#endif
