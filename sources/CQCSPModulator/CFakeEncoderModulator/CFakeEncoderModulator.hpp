#ifndef _C_FAKE_ENCODER_MODULATOR_HPP_
#define _C_FAKE_ENCODER_MODULATOR_HPP_

#include <vector>

#include "CQCSPModulator/CCompleteModulator/CCompleteModulator.hpp"
#include "utilities/definitions.hpp"

namespace QCSP {

/**
 * @brief A dummy modulator that always produce the same frame.
 *
 */
class CFakeEncoderModulator : public CCompleteModulator {
protected:
    virtual void encode(const std::vector<uint_gf_t> & message, std::vector<uint_gf_t> & codeword) override;

public:
    /**
     * @brief Construct a new CFakeEncoderModulator object
     *
     * @param _pn PN sequence
     * @param _om Overmodulation sequence
     */
    CFakeEncoderModulator(const std::vector<int> & _pn, const std::vector<int> & _om);
    virtual ~CFakeEncoderModulator() = default;
};

} // namespace QCSP

#endif // _C_FAKE_ENCODER_MODULATOR_HPP_
