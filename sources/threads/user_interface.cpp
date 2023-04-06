#include <atomic>
#include <iostream>

#include <uhd/utils/thread.hpp>

#include "./user_interface.hpp"

namespace QCSP {

void * user_interface_run(void * arg) {
    uhd::set_thread_priority_safe(-0.1, false);

    ui_arg_t * loc_arg = (ui_arg_t *) arg;

    std::atomic<bool> & bRunning = loc_arg->bRunning;
    bRunning                     = true;
    if (!loc_arg->no_ui) {
        std::cout << ">> Welcome to the interactive QCSP emitter <<\n\nTo stop running, press 'q'" << std::endl;

        while (bRunning) {
            const char a = std::getchar();
            if (a == 'q') {
                std::cout << "Stop signal detected. Good bye!" << std::endl;
                bRunning = false;
            }
        }
    }

    return nullptr;
}

} // namespace QCSP
