#ifndef _HMI_FUNCTIONS_HPP_
#define _HMI_FUNCTIONS_HPP_

#include <vector>

#include <boost/program_options/variables_map.hpp>

#include "CQCSPModulator/CQCSPModulator.hpp"
#include "CSymbolGenerator/CSymbolGenerator.hpp"
#include "utilities/structures.hpp"

namespace QCSP {

int parse_user_input(int argc, char * argv[], boost::program_options::variables_map & vm);

void parse_vm(const boost::program_options::variables_map & vm, emitter_parameters & prm);

void load_settings(
    const std::string &                filename,
    unsigned &                         n_frame,
    unsigned &                         n_s,
    std::vector<int> &                 pn,
    std::vector<int> &                 best_N,
    std::vector<std::complex<float>> & h_filter);

} // namespace QCSP

#endif
