#ifndef _USER_INTERFACE_HPP_
#define _USER_INTERFACE_HPP_

#include <atomic>

namespace QCSP {

/**
 * @brief UI thread arguments
 * 
 */
typedef struct {
    std::atomic<bool> & bRunning;
    bool                no_ui;
} ui_arg_t;

/**
 * @brief UI thread procedure in the PThread format
 * 
 * @param arg ui_arg_t cast to (void *)
 * @return void* nullptr
 */
void * user_interface_run(void * arg);

} // namespace QCSP

#endif
