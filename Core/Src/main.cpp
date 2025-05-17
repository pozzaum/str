#include "main.h"
#include "OS_scheduler.h"
#include "miros.h"
#include <cstdint>
#include <cstdio>


// Stacks para as threads
uint32_t stack_idleThread[32];
uint32_t stack_taskA[32];
uint32_t stack_taskB[32];

// Estruturas das threads
rtos::OSThread threadA;
rtos::OSThread threadB;

// TaskControlBlocks das tarefas
rtos::TaskControlBlock tcbA = {
    .task_func = nullptr,
    .period = 4,
    .wcet = 1,
    .deadline_rel = 4,
    .deadline_abs = 4,
    .activations = 0,
    .conclusions = 0,
	.ready = false,
	.on_wait = false
};

rtos::TaskControlBlock tcbB = {
    .task_func = nullptr,
    .period = 5,
    .wcet = 1,
    .deadline_rel = 5,
    .deadline_abs = 5,
    .activations = 0,
    .conclusions = 0,
	.ready = false,
	.on_wait = false
};

// Funções das tarefas
void taskA_func() {
    while (1) {
        // Simula trabalho da tarefa A
        //printf("Tarefa A executando no tick %lu\n", rtos::OS_tickCount);

        // Marca como concluída
        rtos::mark_task_completed(rtos::OS_thread[rtos::OS_currIdx]->tcb);

        rtos::yield();
    }
}

void taskB_func() {
    while (1) {
        // Simula trabalho da tarefa B
        //printf("Tarefa B executando no tick %lu\n", rtos::OS_tickCount);

        // Marca como concluída
        rtos::mark_task_completed(rtos::OS_thread[rtos::OS_currIdx]->tcb);

        rtos::yield();
    }
}

int main(void) {
    // Inicializa as threads com suas funções e stacks

	rtos::OS_init(stack_idleThread, sizeof(stack_idleThread));

    OSThread_start(&threadA, &taskA_func, stack_taskA, sizeof(stack_taskA));
    OSThread_start(&threadB, &taskB_func, stack_taskB, sizeof(stack_taskB));

    // Associa cada thread ao seu TaskControlBlock
    add_thread_with_task(&threadA, &tcbA);
    add_thread_with_task(&threadB, &tcbB);
    // Inicia o RTOS (loop principal)
    rtos::OS_run();

    // Nunca deve chegar aqui
    while (1) {}
}
