#include "main.h"
#include "OS_scheduler.h"
#include "miros.h"
#include <cstdint>
#include <cstdio>


// stacks para as threads
uint32_t stack_idleThread[32];
uint32_t stack_taskA[32];
uint32_t stack_taskB[32];
uint32_t stack_taskC[32];
uint32_t conta0, conta1, conta2;

// estruturas das threads
rtos::OSThread threadA;
rtos::OSThread threadB;
rtos::OSThread threadC;

rtos::TaskControlBlock tcbA, tcbB, tcbC;

// Funções das tarefas
void taskA_func() {
    while (1) {
    	conta0++;
        //rtos::mark_task_completed(rtos::OS_thread[rtos::OS_currIdx]->tcb);
    }
}

void taskB_func() {
    while (1) {
    	conta1++;
        //rtos::mark_task_completed(rtos::OS_thread[rtos::OS_currIdx]->tcb);
    }
}

void taskC_func() {
    while (1) {
    	conta2++;
        //rtos::mark_task_completed(rtos::OS_thread[rtos::OS_currIdx]->tcb);
    }
}




int main(void) {
    // inicializa as threads com suas funcoes e stacks

	rtos::init_task_control_block(&tcbA, 70, 20, 50);
	rtos::init_task_control_block(&tcbB, 50, 10, 50);
	rtos::init_task_control_block(&tcbC, 100, 10, 50);


	rtos::OS_init(stack_idleThread, sizeof(stack_idleThread));

    OSThread_start(&threadA, &taskA_func, stack_taskA, sizeof(stack_taskA));
    OSThread_start(&threadB, &taskB_func, stack_taskB, sizeof(stack_taskB));
    OSThread_start(&threadC, &taskC_func, stack_taskC, sizeof(stack_taskC));

    // associa cada thread ao seu TaskControlBlock
    add_thread_with_task(&threadA, &tcbA);
    add_thread_with_task(&threadB, &tcbB);
    add_thread_with_task(&threadC, &tcbC);
    // Inicia o RTOS (loop principal)
    rtos::OS_run();

    // Nunca deve chegar aqui
    while (1) {}
}
