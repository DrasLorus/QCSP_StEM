#ifndef _QCSP_MODULATOR_HPP_
#define _QCSP_MODULATOR_HPP_

#include <string>
#include <vector>

#include "utilities/definitions.hpp"
#include "utilities/structures.hpp"

namespace QCSP {

modulator_t mod_from_string(const std::string & in);

class CQCSPModulator {
public:
    static constexpr const unsigned p = QCSP::_LOG2GF_;
    static constexpr const unsigned q = QCSP::_GF_;
    static constexpr const unsigned N = QCSP::_NSYMBOL_;
    static constexpr const unsigned K = QCSP::_KSYMBOL_;

    /**
     * @brief get size of the message in symbols
     *
     * @return constexpr std::size_t
     */
    static constexpr std::size_t message_size() { return K; }

    /**
     * @brief  get size of a symbol in bits
     *
     * @return constexpr std::size_t
     */
    static constexpr std::size_t symbol_size() { return p; }

    /**
     * @brief  get size of a codeword in symbols
     *
     * @return constexpr std::size_t
     */
    static constexpr std::size_t codeword_size() { return N; }

    /**
     * @brief  get size of the pn sequence in chips
     *
     * @return constexpr std::size_t
     */
    static constexpr std::size_t pn_size() { return q; }

    /**
     * @brief  get size of the overmodulation sequence in bits
     *
     * @return constexpr std::size_t
     */
    static constexpr std::size_t om_size() { return N; }

    /**
     * @brief  get size of the output frame in chips
     *
     * @return constexpr std::size_t
     */
    static constexpr std::size_t frame_size() { return N * q; }

    virtual void process(const std::vector<int> & input, std::vector<int> & output) = 0;
};

} // namespace QCSP

#endif // _QCSP_MODULATOR_HPP_
