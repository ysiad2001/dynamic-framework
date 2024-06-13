#ifndef __COMMON_H__
#define __COMMON_H__

#include "stdint.h"

#define __core_0_code(group) __attribute__((section(".core_0_code." group)))
#define __core_1_code(group) __attribute__((section(".core_1_code." group)))
#define __core_0_data(group) __attribute__((section(".core_0_data." group)))
#define __core_1_data(group) __attribute__((section(".core_1_data." group)))

#define __inline_external __attribute__((always_inline))

// #define MEMMAP 0
// #define DEBUG 0
// #define TIMESTAMP 0



#include "timer_functions.h"
#define TIME_MS ((get_core_num() ? timestamp_core_1() : timestamp_core_0()))


#define SCHEDULE_OFFSET 1000000// Keep it to a second (1 million us) to ensure perfect synchronisation
#define CAPACITY 500
#define SIMULATION_TIME 5000

#define TASK_0_PARAM 15//binary search
#define TASK_1_PARAM 31//bitonic sort
#define TASK_2_PARAM 16//bubble sort
#define TASK_3_PARAM 123456//jfdcint param does nothin
#define TASK_4_PARAM 10//recursion
#define TASK_5_PARAM 11//insertion sort



#define TASK_0_PERIOD 100
#define TASK_0_READ_DELAY 0
#define TASK_0_EXEC_DELAY 0
#define TASK_0_WRITE_DELAY 0

#define TASK_1_PERIOD 150
#define TASK_1_READ_DELAY 0
#define TASK_1_EXEC_DELAY 0
#define TASK_1_WRITE_DELAY 0

#define TASK_2_PERIOD 100
#define TASK_2_READ_DELAY 0
#define TASK_2_EXEC_DELAY 0
#define TASK_2_WRITE_DELAY 0

#define TASK_3_PERIOD 300
#define TASK_3_READ_DELAY 0
#define TASK_3_EXEC_DELAY 0
#define TASK_3_WRITE_DELAY 0

#define TASK_4_PERIOD 150
#define TASK_4_READ_DELAY 0
#define TASK_4_EXEC_DELAY 0
#define TASK_4_WRITE_DELAY 0

#define TASK_5_PERIOD 200
#define TASK_5_READ_DELAY 0
#define TASK_5_EXEC_DELAY 0
#define TASK_5_WRITE_DELAY 0

void task0_core0();
void task0_core1();
void task1_core0();
void task1_core1();
void task2_core0();
void task2_core1();
void task3_core0();
void task3_core1();
void task4_core0();
void task4_core1();
void task5_core0();
void task5_core1();

void task0_init();
void task1_init();
void task2_init();
void task3_init();
void task4_init();
void task5_init();

typedef struct
{
    uint64_t read;
    uint64_t exec;
    uint64_t write;
    uint64_t pass;
    uint64_t core;
    uint64_t task;
} printout_t;

typedef struct
{
    printout_t *elements;
    int size;
} printout_stack_t;

void init_printout_stack(printout_stack_t * stack);
void delete_printout_stack(printout_stack_t *stack);
void append_printout_core_0(printout_stack_t *stack, printout_t element);
void append_printout_core_1(printout_stack_t *stack, printout_t element);
void print_printout(printout_stack_t *stack);

#endif