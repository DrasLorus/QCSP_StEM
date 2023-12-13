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
    const std::vector<int> pn_sequence;
    const std::vector<int> om_sequence;

protected:
    virtual void encode(const std::vector<int> & message, std::vector<int> & codeword);
    virtual void modulate(const std::vector<int> & codeword, std::vector<int> & ccsk_frame);
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
