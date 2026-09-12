#ifndef _TIMER_HPP_
#define _TIMER_HPP_

#include <atomic>

namespace QCSP {

/**
 * @brief Aguments of the timer thread
 *
 */
typedef struct {
    bool                time_limited;
    double              dbl_ttl_us;
    std::atomic<bool> & bTimeNotReached;
} timer_arg_t;

/**
 * @brief timer thread procedure in PThread format
 *
 * @param arg a timer_arg_t cast to (void *)
 * @return void* nullptr
 */
void * timer_run(void * arg);

} // namespace QCSP

#endif // _TIMER_HPP_
