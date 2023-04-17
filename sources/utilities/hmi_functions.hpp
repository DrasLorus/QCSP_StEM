#ifndef _HMI_FUNCTIONS_HPP_
#define _HMI_FUNCTIONS_HPP_

#include <boost/program_options/variables_map.hpp>

#include <vector>

namespace QCSP {

int parse_user_input(int argc, char ** argv, boost::program_options::variables_map & vm);

void load_settings(
    const std::string &                filename,
    unsigned &                         n_frame,
    unsigned &                         n_s,
    std::vector<int> &                 pn,
    std::vector<int> &                 best_N,
    std::vector<std::complex<float>> & h_filter);

} // namespace QCSP

#endif
