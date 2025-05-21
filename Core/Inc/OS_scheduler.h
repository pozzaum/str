#ifndef INC_OS_SCHEDULER_H_
#define INC_OS_SCHEDULER_H_

#include <cstdint>
#include "miros.h"

namespace rtos{

extern void yield(void);

extern void add_thread_with_task(OSThread* thread, TaskControlBlock* task);

extern void update_ready_tasks(void);

extern void mark_task_completed(TaskControlBlock* task);

extern void update_task_deadlines(void);

extern void monitor_overruns(void);

extern void init_task_control_block(TaskControlBlock* tcb, uint32_t period, uint32_t wcet, uint32_t deadline_rel);

extern void OS_scheduler(void);

}

#endif /* INC_OS_SCHEDULER_H_ */
