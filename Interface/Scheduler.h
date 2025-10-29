/*******************************************************************************
* Copyright (c) 2025.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_SCHEDULER_H
#define FINEMOTE_SCHEDULER_H

typedef void (*TaskFunc_t)();
#define TASK_EXPORT(n) __attribute__((used, section("Tasks"))) const TaskFunc_t __##n = n

void FineMoteScheduler();

#endif
