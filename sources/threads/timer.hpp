#ifndef _TIMER_HPP_
#define _TIMER_HPP_

#include <atomic>

namespace QCSP {

typedef struct {
    bool                time_limited;
    double              dbl_ttl_us;
    std::atomic<bool> & bTimeNotReached;
} timer_arg_t;

void * timer_run(void * arg);

}

#endif // _TIMER_HPP_
