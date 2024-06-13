
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include "hardware/structs/bus_ctrl.h"
#include "hardware/regs/busctrl.h"
#include "hardware/structs/xip_ctrl.h"
#include "hardware/regs/xip.h"
#include "pico/multicore.h"
#include "pico/platform.h"
#include "hardware/structs/scb.h"
#include "common.h"
#include "timer_functions.h"
#include "locks.h"
#include <inttypes.h>

// Define hardware addresses
#define bus_perf_0_hw ((bus_ctrl_perf_hw_t *)(BUSCTRL_BASE + BUSCTRL_PERFCTR0_OFFSET))
#define bus_perf_1_hw ((bus_ctrl_perf_hw_t *)(BUSCTRL_BASE + BUSCTRL_PERFCTR1_OFFSET))
#define bus_perf_2_hw ((bus_ctrl_perf_hw_t *)(BUSCTRL_BASE + BUSCTRL_PERFCTR2_OFFSET))
#define bus_perf_3_hw ((bus_ctrl_perf_hw_t *)(BUSCTRL_BASE + BUSCTRL_PERFCTR3_OFFSET))

// Only alarm 0 be used
#define ALARM_TASK_RELEASE 0
#define ALARM_TASK_IRQ TIMER_IRQ_0

#define ALARM_FINISH_SIMULATION 1
#define ALARM_FINISH_IRQ TIMER_IRQ_1

// When a task is released, set to 1, when it is taken by a core, reset to 0
// Everything modifying this should be spin locked
// You may want to set up an array for this, but doing that will require an additional pointer pointing to array, causing contention (Maybe there are way to get around this)
__scratch_x("flag") uint32_t volatile task_release_flag_0;
__scratch_x("flag") uint32_t volatile task_release_flag_1;
__scratch_x("flag") uint32_t volatile task_release_flag_2;
__scratch_x("flag") uint32_t volatile task_release_flag_3;
__scratch_x("flag") uint32_t volatile task_release_flag_4;
__scratch_x("flag") uint32_t volatile task_release_flag_5;

// When a task is released, add 1, when it is finished by a core, subtract 1. If the flag is not 0 when a task is released, it means ddl miss occured
// Everything modifying this should be spin locked
__scratch_x("flag") uint32_t volatile task_finish_flag_0;
__scratch_x("flag") uint32_t volatile task_finish_flag_1;
__scratch_x("flag") uint32_t volatile task_finish_flag_2;
__scratch_x("flag") uint32_t volatile task_finish_flag_3;
__scratch_x("flag") uint32_t volatile task_finish_flag_4;
__scratch_x("flag") uint32_t volatile task_finish_flag_5;

// Used to record the ddl miss task
__scratch_y("flag") uint32_t miss = -1;

// VTOR table for core 1
uint32_t __attribute__((section(".ram_vector_table_core_1"))) vector_table_core_1[48] = {0};

// Function prototype
static void __core_0_code("irq")(alarm_task_irq)(void);
void __core_0_code("scheduler")(scheduler_core_0)();
void __core_1_code("scheduler")(scheduler_core_1)();
void system_init();

typedef struct
{
    uint32_t period;                 // The period of the task
    uint32_t next_release;           // The next time the task should be released, initialize to first release
    uint32_t read_delay;             // After a read, sleep by x us
    uint32_t exec_delay;             // After a exec, sleep by x us
    uint32_t write_delay;            // After a write, sleep by x us
    uint32_t volatile *release_flag; // A pointer to the task's release flag
    uint32_t volatile *finish_flag;  // A pointer to the task's finish flag
} task_t;

__core_0_data("scheduler") task_t tasks[] = {
    {.period = TASK_0_PERIOD, .next_release = TASK_0_PERIOD + SCHEDULE_OFFSET, .release_flag = &task_release_flag_0, .finish_flag = &task_finish_flag_0},
    {.period = TASK_1_PERIOD, .next_release = TASK_1_PERIOD + SCHEDULE_OFFSET, .release_flag = &task_release_flag_1, .finish_flag = &task_finish_flag_1},
    {.period = TASK_2_PERIOD, .next_release = TASK_2_PERIOD + SCHEDULE_OFFSET, .release_flag = &task_release_flag_2, .finish_flag = &task_finish_flag_2},
    {.period = TASK_3_PERIOD, .next_release = TASK_3_PERIOD + SCHEDULE_OFFSET, .release_flag = &task_release_flag_3, .finish_flag = &task_finish_flag_3},
    {.period = TASK_4_PERIOD, .next_release = TASK_4_PERIOD + SCHEDULE_OFFSET, .release_flag = &task_release_flag_4, .finish_flag = &task_finish_flag_4},
    {.period = TASK_5_PERIOD, .next_release = TASK_5_PERIOD + SCHEDULE_OFFSET, .release_flag = &task_release_flag_5, .finish_flag = &task_finish_flag_5},

};

void __core_0_code("irq")(alarm_task_irq)(void)
{
    hw_clear_bits(&timer_hw->intr, 1u << ALARM_TASK_RELEASE); // Clear the irq
    uint32_t curr_time = timer_hw->timerawl;
    uint64_t next_alarm = curr_time + 1 << 31;
    for (int i = 0; i < sizeof(tasks) / sizeof(task_t); ++i)
    {

        if (tasks[i].next_release <= curr_time) // Found a task to be released
        {
            uint32_t irq;
            irq = spin_lock_blocking(LOCK_0);
            if (*tasks[i].finish_flag) // task i has missed ddl
            {
                printf("Miss in %d at time %d\n", i, curr_time);
            }
            ++(*tasks[i].release_flag); // Release the task
            ++(*tasks[i].finish_flag);
            spin_unlock(LOCK_0, irq);

            tasks[i].next_release += tasks[i].period; // Schedule the next release of task i
        }
        next_alarm = (next_alarm < tasks[i].next_release) ? next_alarm : tasks[i].next_release; // Next time alarm fire is the smallest of all (tasks[i].next_release)
    }

    curr_time = timer_hw->timerawl;
    timer_hw->alarm[ALARM_TASK_RELEASE] = next_alarm; // Prime the next alarm
}
void main_core_1(void)
{
    scheduler_core_1();
}

void system_init()
{
    // Turn off XIP cache
    xip_ctrl_hw->ctrl = (xip_ctrl_hw->ctrl & ~XIP_CTRL_EN_BITS);

    // Counter init to count contested accesses

    // Shared group
    bus_perf_0_hw->sel = arbiter_sram4_perf_event_access_contested;
    bus_perf_1_hw->sel = arbiter_sram5_perf_event_access_contested;
    bus_perf_2_hw->sel = arbiter_xip_main_perf_event_access_contested;
    bus_perf_3_hw->sel = arbiter_rom_perf_event_access_contested;

    // Local group
    // bus_perf_0_hw->sel = arbiter_sram0_perf_event_access_contested;
    // bus_perf_1_hw->sel = arbiter_sram1_perf_event_access_contested;
    // bus_perf_2_hw->sel = arbiter_sram2_perf_event_access_contested;
    // bus_perf_3_hw->sel = arbiter_sram3_perf_event_access_contested;

    // The content of VTOR table cannot be random, get the content from the core 0 VTOR table
    // But set the first word to start of stack1 (must be so according to the documentation)
    memcpy(vector_table_core_1, (const uint32_t *)scb_hw->vtor, sizeof(vector_table_core_1));
    vector_table_core_1[0] = 0x21040000;

    // When needing spin_lock, always call them by their absolute address defined in locks.h to avoid accessing a variable, which cause contention.
    spin_lock_claim(16);
    spin_lock_claim(17);
    spin_lock_claim(18);
    spin_lock_claim(19);

    // Initialize these flags
    task_finish_flag_0 = 0;
    task_finish_flag_1 = 0;
    task_finish_flag_2 = 0;
    task_finish_flag_3 = 0;
    task_finish_flag_4 = 0;
    task_finish_flag_5 = 0;

    task_release_flag_0 = 0;
    task_release_flag_1 = 0;
    task_release_flag_2 = 0;
    task_release_flag_3 = 0;
    task_release_flag_4 = 0;
    task_release_flag_5 = 0;

    // Prime the first alarm
    timer_hw->alarm[ALARM_TASK_RELEASE] = SCHEDULE_OFFSET;
    timer_hw->alarm[ALARM_FINISH_SIMULATION] = SCHEDULE_OFFSET + SIMULATION_TIME;
}

void __core_0_code("scheduler")(scheduler_core_0)()
{

    // reset the counter
    bus_perf_0_hw->value = 0;
    bus_perf_1_hw->value = 0;
    bus_perf_2_hw->value = 0;
    bus_perf_3_hw->value = 0;

    uint32_t irq;
    while (1)
    {
        irq = spin_lock_blocking(LOCK_0);

        if (task_release_flag_0) // This task has been released
        {
            --task_release_flag_0; // Clear this task's release flag
            spin_unlock(LOCK_0, irq);
            task0_core0(); // Execute task 0 on task 0
            irq = spin_lock_blocking(LOCK_0);
            --task_finish_flag_0; // Clear this task's finish flag
            spin_unlock(LOCK_0, irq);
        }
        else if (task_release_flag_1)
        {
            --task_release_flag_1;
            spin_unlock(LOCK_0, irq);
            task1_core0();
            irq = spin_lock_blocking(LOCK_0);
            --task_finish_flag_1;
            spin_unlock(LOCK_0, irq);
        }
        else if (task_release_flag_2)
        {
            --task_release_flag_2;
            spin_unlock(LOCK_0, irq);
            task2_core0();
            irq = spin_lock_blocking(LOCK_0);
            --task_finish_flag_2;
            spin_unlock(LOCK_0, irq);
        }
        else if (task_release_flag_3)
        {
            --task_release_flag_3;
            spin_unlock(LOCK_0, irq);
            task3_core0();
            irq = spin_lock_blocking(LOCK_0);
            --task_finish_flag_3;
            spin_unlock(LOCK_0, irq);
        }
        else if (task_release_flag_4)
        {
            --task_release_flag_4;
            spin_unlock(LOCK_0, irq);
            task4_core0();
            irq = spin_lock_blocking(LOCK_0);
            --task_finish_flag_4;
            spin_unlock(LOCK_0, irq);
        }
        else if (task_release_flag_5)
        {
            --task_release_flag_5;
            spin_unlock(LOCK_0, irq);
            task5_core0();
            irq = spin_lock_blocking(LOCK_0);
            --task_finish_flag_5;
            spin_unlock(LOCK_0, irq);
        }
        else
        {
            spin_unlock(LOCK_0, irq);
        }
        sleep_ms_core_0(1); // Prevent one of the cores from running too fast
    }
}

void __core_1_code("scheduler")(scheduler_core_1)()
{

    // reset the counter
    bus_perf_0_hw->value = 0;
    bus_perf_1_hw->value = 0;
    bus_perf_2_hw->value = 0;
    bus_perf_3_hw->value = 0;
    uint32_t irq;
    while (1)
    {
        irq = spin_lock_blocking(LOCK_0);
        if (task_release_flag_0)
        {
            --task_release_flag_0;
            spin_unlock(LOCK_0, irq);
            task0_core1();
            irq = spin_lock_blocking(LOCK_0);
            --task_finish_flag_0;
            spin_unlock(LOCK_0, irq);
        }
        else if (task_release_flag_1)
        {
            --task_release_flag_1;
            spin_unlock(LOCK_0, irq);
            task1_core1();
            irq = spin_lock_blocking(LOCK_0);
            --task_finish_flag_1;
            spin_unlock(LOCK_0, irq);
        }
        else if (task_release_flag_2)
        {
            --task_release_flag_2;
            spin_unlock(LOCK_0, irq);
            task2_core1();
            irq = spin_lock_blocking(LOCK_0);
            --task_finish_flag_2;
            spin_unlock(LOCK_0, irq);
        }
        else if (task_release_flag_3)
        {
            --task_release_flag_3;
            spin_unlock(LOCK_0, irq);
            task3_core1();
            irq = spin_lock_blocking(LOCK_0);
            --task_finish_flag_3;
            spin_unlock(LOCK_0, irq);
        }
        else if (task_release_flag_4)
        {
            --task_release_flag_4;
            spin_unlock(LOCK_0, irq);
            task4_core1();
            irq = spin_lock_blocking(LOCK_0);
            --task_finish_flag_4;
            spin_unlock(LOCK_0, irq);
        }
        else if (task_release_flag_5)
        {
            --task_release_flag_5;
            spin_unlock(LOCK_0, irq);
            task5_core1();
            irq = spin_lock_blocking(LOCK_0);
            --task_finish_flag_5;
            spin_unlock(LOCK_0, irq);
        }
        else
        {
            spin_unlock(LOCK_0, irq);
        }
        sleep_ms_core_1(1);
    }
}

printout_t __core_0_data("print") printout_core_0[CAPACITY];
printout_t __core_1_data("print") printout_core_1[CAPACITY];

__core_0_data("print") printout_stack_t printout_stack_core_0 = {.elements = printout_core_0, .size = 0};
__core_1_data("print") printout_stack_t printout_stack_core_1 = {.elements = printout_core_1, .size = 0};

__core_0_code("print") void finish_irq()
{
    
    hw_clear_bits(&timer_hw->intr, 1u << ALARM_FINISH_SIMULATION); // Clear the irq

    printf("\n\nSimulation finished,current time is %lu\n", timer_hw->timerawl);

    print_printout(&printout_stack_core_0);
    print_printout(&printout_stack_core_1);

    printf("END of printing\n");
    while (1)
        ;
}

int main()
{
    stdio_init_all();
    system_init();
    task0_init();
    task1_init();
    task2_init();
    task3_init();
    task4_init();
    task5_init();
#ifdef DEBUG
    printf("Hallo framework!\n");
#endif

    multicore_lauch_core1_separate_vtor_table(main_core_1, (uint32_t)vector_table_core_1); // Launch core 0
    sleep_ms_core_0(100000);                                                               // wait slightly to ensure core 1 is def set up

    // Set up alarm
    hw_set_bits(&timer_hw->inte, 1u << ALARM_TASK_RELEASE);
    irq_set_exclusive_handler(ALARM_TASK_IRQ, alarm_task_irq);
    irq_set_enabled(ALARM_TASK_IRQ, true);

    
    irq_set_exclusive_handler(ALARM_FINISH_IRQ, finish_irq);
    hw_set_bits(&timer_hw->inte, 1u << ALARM_FINISH_IRQ);
    irq_set_enabled(ALARM_FINISH_IRQ, true);

    // Start core 0 schedule
    scheduler_core_0();
    return 0;
}
