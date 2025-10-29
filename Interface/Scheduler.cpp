/*******************************************************************************
* Copyright (c) 2025.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "Scheduler.h"

void FineMoteScheduler() {
    extern const TaskFunc_t Tasks$$Base;
    extern const TaskFunc_t Tasks$$Limit;

    const TaskFunc_t *funcPtr;
    for(funcPtr = &Tasks$$Base; funcPtr < &Tasks$$Limit; funcPtr++) {
        (*funcPtr)();
    }
}
