
#include <vector>

namespace QCSP {

/**
 * @brief Utility function to convert 8-bit char to arbitrary-sized integers
 * 
 * @tparam TBits Number of bits per integer 
 * @tparam TSymbols Number of bytes per symbols
 * @param input a vector of \f$\frac{TSymbols \times TBits}{8}\f$ bytes
 * @param output a vector of \f$\frac{TSymbols \times TBits}{8}\f$ bytes
 */
template <int TBits, int TSymbols>
void bytes_to_int(const std::vector<char> & input, std::vector<int> & output);

} // namespace QCSP
