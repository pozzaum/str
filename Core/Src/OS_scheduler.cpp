#include "OS_scheduler.h"

namespace rtos{

void add_thread_with_task(OSThread* thread, TaskControlBlock* task) {
	thread->tcb = task; // Associa a task à thread
	OS_thread[OS_threadNum++] = thread; // Adiciona a thread ao array de threads
}


void remove_task(std::vector<TaskControlBlock>& tasks, uint8_t task_id) {
	if (task_id < tasks.size()) {
		tasks.erase(tasks.begin() + task_id);
	}
}


void update_ready_tasks(void) {
	for (uint8_t i = 1U; i <= OS_threadNum; i++) {
		TaskControlBlock* tcb = OS_thread[i]->tcb;
		if (((OS_tickCount / tcb->period) > tcb->activations) && (tcb->activations == tcb->conclusions)){	//verifica se passou um periodo de ativacao
			tcb->activations++;
			tcb->ready = true;
			OS_readySet |= (1U << (i-1U));
		}
	}
}


void update_task_deadlines(void) {
	for (uint8_t i = 1U; i <= OS_threadNum; i++) {
		TaskControlBlock* tcb = OS_thread[i]->tcb;
		if (tcb->ready) {
			tcb->deadline_abs = OS_tickCount + tcb->deadline_rel; // atualiza a próxima deadline
		}
	}
}


void mark_task_completed(TaskControlBlock*task){
	if (task != nullptr){
		task->ready = false;
		task->conclusions++;
	}
}


void OS_scheduler(void) {
	if (OS_readySet == 0U) {
		OS_currIdx = 0U; // Thread idle
		OS_next = &idleThread;
	} else {
		uint32_t min_deadline = UINT32_MAX;
		uint8_t next_thread_idx = 0;

		for (uint8_t i = 1; i <= OS_threadNum; i++) {
			if (OS_readySet & (1U << (i - 1U))) {	// verifica se a thread está pronta
				TaskControlBlock* tcb = OS_thread[i]->tcb;	// Obter a task associada
				if (tcb->deadline_abs < min_deadline) {
					min_deadline = tcb->deadline_abs;
					next_thread_idx = i;
				}
			}
		}

		OS_currIdx = next_thread_idx;
		OS_next = OS_thread[OS_currIdx];
	}

	if (OS_next != OS_curr) {
		*(uint32_t volatile *)0xE000ED04 = (1U << 28); // PendSV para troca de contexto
	}
}

}
