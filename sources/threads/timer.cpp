#include <atomic>
#include <iostream>
#include <chrono>

#include <uhd/utils/thread.hpp>

#include "./timer.hpp"

namespace QCSP {

void * timer_run(void * arg) {
    timer_arg_t * loc_arg = (timer_arg_t *) arg;

    bool                time_limited    = loc_arg->time_limited;
    double              dbl_ttl_us      = loc_arg->dbl_ttl_us;
    std::atomic<bool> & bTimeNotReached = loc_arg->bTimeNotReached;

    if (time_limited) {
        uhd::set_thread_priority_safe(0, true);

        size_t uint_ttl_s  = size_t(dbl_ttl_us * 1e-6);
        size_t uint_ttl_ms = size_t((dbl_ttl_us - double(uint_ttl_s) * 1e6) * 1e-3);
        size_t uint_ttl_us = size_t(dbl_ttl_us - double(uint_ttl_s) * 1e6 - double(uint_ttl_ms) * 1e3);

        std::chrono::seconds      ttl_s(uint_ttl_s);
        std::chrono::milliseconds ttl_ms(uint_ttl_ms);
        std::chrono::microseconds ttl_us(uint_ttl_us);

        // cout << uint_ttl_s << "s " << uint_ttl_ms << "ms " << uint_ttl_us << "us" << endl;
        bTimeNotReached = true;
        std::this_thread::sleep_for(ttl_s);
        // cout << "s" << endl;
        std::this_thread::sleep_for(ttl_ms);
        // cout << "ms" << endl;
        std::this_thread::sleep_for(ttl_us);
        // cout << "us" << endl;
        bTimeNotReached = false;
        std::cout << "Time limit reached." << std::endl;
    }
    return nullptr;
}

}
