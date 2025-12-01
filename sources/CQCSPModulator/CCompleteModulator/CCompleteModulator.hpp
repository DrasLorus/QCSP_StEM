/**
 * @file CCompleteModulator.hpp
 * @author Camille Monière (camille.moniere@univ-ubs.fr)
 * @brief
 * @version 0.1
 * @date 2023-12-13
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef _COMPLETE_MODULATOR_HPP_
#define _COMPLETE_MODULATOR_HPP_

#include <cmath>
#include <cstdint>
#include <vector>

#include "CQCSPModulator/CQCSPModulator.hpp"

namespace QCSP {

/**
 * @brief THe reference implementation of a complete working QCSP modulator
 *
 */
class CCompleteModulator : public CQCSPModulator {
private:
    /// @brief PN sequence of size q
    const std::vector<int> pn_sequence;
    /// @brief Overmodulation sequence of size N
    const std::vector<int> om_sequence;

protected:
    /**
     * @brief Encode a message into a codeword
     *
     * @param message of K symbols in GF(_GF_)
     * @param codeword of N symbols in GF(_GF_)
     */
    virtual void encode(const std::vector<uint_gf_t> & message, std::vector<int> & codeword);

    /**
     * @brief Modulate a codeword into a ccsk_frame
     *
     * @param codeword of N symbols in GF(_GF_)
     * @param ccsk_frame of N \f$\times\f$ q symbols in {-1 ; 1}
     */
    virtual void modulate(const std::vector<int> & codeword, std::vector<int> & ccsk_frame);

    /**
     * @brief Overodulate a ccsk_frame into a QCSP frame
     *
     * @param ccsk_frame of N \f$\times\f$ q symbols in {-1 ; 1}
     * @param frame of N \f$\times\f$ q symbols in {-1 ; 1} overmodulated by the om_sequence
     */
    virtual void overmodulate(const std::vector<int> & ccsk_frame, std::vector<int> & frame);

public:
    virtual void process(const std::vector<int> & input, std::vector<int> & output) override;

    /**
     * @brief Construct a new CCompleteModulator object
     *
     * @param _pn
     * @param _om
     */
    CCompleteModulator(const std::vector<int> & _pn, const std::vector<int> & _om);

    /**
     * @brief Destroy the CCompleteModulator object
     *
     */
    virtual ~CCompleteModulator() = default;
};

} // namespace QCSP

#endif // _COMPLETE_MODULATOR_HPP_
