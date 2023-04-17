/**
 * @file CTimerGenerator.hpp
 * @author Camille Monière (camille.moniere@univ-ubs.fr)
 * @brief
 * @version 0.1
 * @date 2023-04-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef _QCSP_TIMER_GENERATOR_HPP_
#define _QCSP_TIMER_GENERATOR_HPP_

#include <atomic>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>

#include "CSymbolGenerator/CSymbolGenerator.hpp"

namespace QCSP {

class CTimerGenerator : public CSymbolGenerator {
private:
    uint16_t       counter;

public:
    CTimerGenerator();
    virtual ~CTimerGenerator() = default;

    virtual void process(std::vector<int> & message) override;
};

} // namespace QCSP

#endif // _QCSP_TIMER_GENERATOR_HPP_
