#include "stdint.h"
#include "common.h"
#include <string.h>
#include <stdio.h>


__core_0_code("print") void append_printout_core_0(printout_stack_t *stack, printout_t element)
{
    if (stack->size == CAPACITY)
    {
        printf("ERROR: Stack is full\n");
    }
    stack->elements[stack->size++] = element;
}

__core_1_code("print") void append_printout_core_1(printout_stack_t *stack, printout_t element)
{
    if (stack->size == CAPACITY)
    {
        printf("ERROR: Stack is full\n");
    }
    stack->elements[stack->size++] = element;
}


void print_printout(printout_stack_t *stack)
{
int periods[] = {TASK_0_PERIOD, TASK_1_PERIOD, TASK_2_PERIOD, TASK_3_PERIOD, TASK_4_PERIOD, TASK_5_PERIOD};

    for (int i = 0; i < stack->size; i++)
    {
        int taskIndex = (stack->elements[i].task);
        printf("\nCORE_%lli, TASK_%lli\n Release: %lli, Read: %lli, execute: %lli, write: %lli, pass: %lli\n",
        stack->elements[i].core, 
        stack->elements[i].task, 
        ((stack->elements[i].read-SCHEDULE_OFFSET)/periods[taskIndex]) * (periods[taskIndex]), 
        stack->elements[i].read - SCHEDULE_OFFSET, 
        stack->elements[i].exec - SCHEDULE_OFFSET, 
        stack->elements[i].write - SCHEDULE_OFFSET, 
        stack->elements[i].pass - SCHEDULE_OFFSET);
    }
}