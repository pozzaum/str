#include "main.h"
#include "OS_scheduler.h"

namespace rtos{

void yield(void) {
    __disable_irq();
    OS_sched(); // chama o escalonador para escolher a próxima thread
    __enable_irq();
}

void add_thread_with_task(OSThread* thread, TaskControlBlock* task) {
	thread->tcb = task; // associa a task a thread
	thread->tcb->on_wait = true;
	thread->tcb->ready = true;
	thread->tcb->activations = 1;
}


void update_ready_tasks(void) {	//a cada chamada de OS_tick, o estado das tasks e verificado
	for (uint8_t i = 1U; i <= OS_threadNum; i++) {
		TaskControlBlock* tcb = OS_thread[i]->tcb;
		if (((OS_tickCount / tcb->period) >= tcb->activations) && (tcb->activations == tcb->conclusions)){	//verifica se passou um periodo de ativacao
			tcb->activations++;
			tcb->ready = true;
			OS_readySet |= (1U << (i-1U));
		}
	}
}


void update_task_deadlines(void) {
	for (uint8_t i = 1U; i <= OS_threadNum; i++) {
		TaskControlBlock* tcb = OS_thread[i]->tcb;
		if (tcb->ready && tcb->on_wait) {
			tcb->deadline_abs = OS_tickCount + tcb->deadline_rel; // atualiza a próxima deadline
			tcb->on_wait = false;
		}
	}
}


void mark_task_completed(TaskControlBlock* task) {
    if (task != nullptr) {
        task->ready = false;
        task->on_wait = true;
        task->conclusions++;
        // procura o indice da thread correspondente
        for (uint8_t i = 0; i <= OS_threadNum; i++) {
            if (OS_thread[i]->tcb == task) {
                OS_readySet &= ~(1U << (i - 1U)); // limpa o bit
                break;
            }
        }
    }
}


void monitor_overruns(void){
	if ((OS_tickCount % OS_curr->tcb->period) > OS_curr->tcb->wcet){
		OS_curr->tcb->ready = false;
		OS_curr->tcb->on_wait = true;
		OS_curr->tcb->conclusions++;
		OS_curr->tcb->overruns++;
		OS_readySet &= ~(1U << (OS_currIdx - 1U));
		OS_sched();
	}
}

void OS_scheduler(void) {
	if (OS_readySet == 0U) {
		OS_currIdx = 0U; // Thread idle
		OS_next = &idleThread;
	} else {
		uint32_t min_deadline = UINT32_MAX;
		uint8_t next_thread_idx = 0;
		bool idle_case = true;

		for (uint8_t i = 1U; i <= OS_threadNum; i++) {
			TaskControlBlock* tcb = OS_thread[i]->tcb;	// obter a task associada
			if (tcb->ready) {	// verifica se a thread está pronta
				if (tcb->deadline_abs < min_deadline) {
					min_deadline = tcb->deadline_abs;
					next_thread_idx = i;
				}
				idle_case = false;
			}
		}

		OS_currIdx = next_thread_idx;
		OS_next = OS_thread[OS_currIdx];
		if(idle_case) OS_next = &idleThread;
	}

	if (OS_next != OS_curr) {
		*(uint32_t volatile *)0xE000ED04 = (1U << 28); // PendSV para troca de contexto
	}
}

}
