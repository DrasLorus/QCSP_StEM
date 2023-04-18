#ifndef _USER_INTERFACE_HPP_
#define _USER_INTERFACE_HPP_

#include <atomic>

namespace QCSP {

typedef struct {
    std::atomic<bool> & bRunning;
    bool                no_ui;
} ui_arg_t;

void * user_interface_run(void * arg);

} // namespace QCSP

#endif
