
#include <string.h>
#include <stdio.h>

#include "pico/platform.h"
#include "pico/multicore.h"
#include "timer_functions.h"

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

#include "timer_functions.h"
#include "locks.h"
#include "common.h"


typedef struct
{
    uint32_t arr[TASK_2_PARAM];
} data_in_t;

typedef struct
{
    uint32_t sorted_arr[TASK_2_PARAM];
    uint32_t complete;
} data_out_t;

/*
    Local data type definition.
    DO NOT MODIFY!
*/
typedef struct
{
    data_in_t local_data_in;
    data_out_t local_data_out;
} local_data_t;

extern printout_stack_t printout_stack_core_0;
extern printout_stack_t printout_stack_core_1;

static __scratch_y("task2") data_in_t data_in;
static __scratch_y("task2") data_out_t data_out;

void task2_init()
{
    for (int i = 0; i < 14; ++i)
    {
        data_in.arr[i] = i;
    }
    for (int i = 15; i < TASK_1_PARAM; ++i)
    {
        data_in.arr[i] = 1145 - i;
    }
    data_in.arr[14] = 66;
}
uint32_t __core_0_code("task2") bsort_return_core0(uint32_t *arr)
{
    uint32_t Sorted = 1;
    uint32_t Index;

    for (Index = 0; Index < TASK_2_PARAM - 1; Index++)
        Sorted = Sorted && (arr[Index] < arr[Index + 1]);

    return 1 - Sorted;
}

uint32_t __core_0_code("task2") bsort_BubbleSort_core0(uint32_t *arr)
{
    uint32_t Sorted = 0;
    uint32_t Temp, Index, i;

    for (i = 0; i < TASK_2_PARAM - 1; i++)
    {
        Sorted = 1;
        for (Index = 0; Index < TASK_2_PARAM - 1; Index++)
        {
            if (Index > TASK_2_PARAM - i)
                break;
            if (arr[Index] > arr[Index + 1])
            {
                Temp = arr[Index];
                arr[Index] = arr[Index + 1];
                arr[Index + 1] = Temp;
                Sorted = 0;
            }
        }

        if (Sorted)
            break;
    }

    return 0;
}

uint32_t __core_0_code("task2") task2_exec_core0(local_data_t *local_data)
{
    uint32_t arr[16];
    for (uint32_t i = 0; i < 16; ++i)
    {
        arr[i] = local_data->local_data_in.arr[i];
    }
    bsort_BubbleSort_core0(arr);
    for (uint32_t i = 0; i < 16; ++i)
    {
        local_data->local_data_out.sorted_arr[i] = arr[i];
    }
    local_data->local_data_out.complete = bsort_return_core0(arr);
}

void __core_0_code("task2")(task2_core0)()
{
    uint32_t irq;
#ifdef DEBUG
    printf("task_2 entered \n");
#endif
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_READ = TIME_MS;
#endif
    // READ PHASE
    local_data_t local_data;
    memcpy(&local_data.local_data_in, &data_in, sizeof(data_in));
    sleep_ms_core_0(TASK_2_READ_DELAY);
    __UNLOCK_MEM__
#ifdef MEMMAP
    printf("task_2 Data in (should be SRAM_5) address, size: %p: %p\n", &data_in, sizeof(data_in));
    printf("task_2 Data out (should be SRAM_5) address, size: %p: %p\n", &data_out, sizeof(data_out));
    printf("task_2 Local data IN address: %p\n", &local_data.local_data_in);
    printf("task_2 Local data OUT address: %p\n", &local_data.local_data_out);
#endif
    // READ PHASE END
#ifdef TIMESTAMP
    uint64_t timestamp_EXECUTE = TIME_MS;
#endif
    // EXEC PHASE
    task2_exec_core0(&local_data);
    sleep_ms_core_0(TASK_2_EXEC_DELAY);
    // EXEC PHASE END
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_WRITE = TIME_MS;
#endif
    // WRITE PHASE
    memcpy(&data_out, &local_data.local_data_out, sizeof(data_out));
    // WRITE PHASE END
    sleep_ms_core_0(TASK_2_WRITE_DELAY);
    __UNLOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_PASS = TIME_MS;
    // printf("\nCORE_0, TASK_2\nRead: %lli, execute: %lli, write: %lli, pass: %lli\n", timestamp_READ, timestamp_EXECUTE, timestamp_WRITE, timestamp_PASS);
    append_printout_core_0(&printout_stack_core_0, (printout_t){.read = timestamp_READ, .exec = timestamp_EXECUTE, .write = timestamp_WRITE, .pass = timestamp_PASS, .core = 0, .task = 2});
#endif
#ifdef DEBUG
    printf("core_0 task_2: Return value: %d (should be 0)\n", data_out.complete);
#endif
}

uint32_t __core_1_code("task2") bsort_return_core1(uint32_t *arr)
{
    uint32_t Sorted = 1;
    uint32_t Index;

    for (Index = 0; Index < TASK_2_PARAM - 1; Index++)
        Sorted = Sorted && (arr[Index] < arr[Index + 1]);

    return 1 - Sorted;
}

uint32_t __core_1_code("task2") bsort_BubbleSort_core1(uint32_t *arr)
{
    uint32_t Sorted = 0;
    uint32_t Temp, Index, i;

    for (i = 0; i < TASK_2_PARAM - 1; i++)
    {
        Sorted = 1;
        for (Index = 0; Index < TASK_2_PARAM - 1; Index++)
        {
            if (Index > TASK_2_PARAM - i)
                break;
            if (arr[Index] > arr[Index + 1])
            {
                Temp = arr[Index];
                arr[Index] = arr[Index + 1];
                arr[Index + 1] = Temp;
                Sorted = 0;
            }
        }

        if (Sorted)
            break;
    }

    return 0;
}

uint32_t __core_1_code("task2") task2_exec_core1(local_data_t *local_data)
{
    uint32_t arr[16];
    for (uint32_t i = 0; i < 16; ++i)
    {
        arr[i] = local_data->local_data_in.arr[i];
    }
    bsort_BubbleSort_core1(arr);
    for (uint32_t i = 0; i < 16; ++i)
    {
        local_data->local_data_out.sorted_arr[i] = arr[i];
    }
    local_data->local_data_out.complete = bsort_return_core1(arr);
}

void __core_1_code("task2")(task2_core1)()
{
#ifdef DEBUG
    printf("task_2 entered \n");
#endif
    uint32_t irq;
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_READ = TIME_MS;
#endif
    // READ PHASE
    local_data_t local_data;
    memcpy(&local_data.local_data_in, &data_in, sizeof(data_in));
    sleep_ms_core_1(TASK_2_READ_DELAY);
    __UNLOCK_MEM__
#ifdef MEMMAP
    printf("task_2 Data in (should be SRAM_5) address, size: %p: %p\n", &data_in, sizeof(data_in));
    printf("task_2 Data out (should be SRAM_5) address, size: %p: %p\n", &data_out, sizeof(data_out));
    printf("task_2 Local data IN address: %p\n", &local_data.local_data_in);
    printf("task_2 Local data OUT address: %p\n", &local_data.local_data_out);
#endif
    // READ PHASE END
#ifdef TIMESTAMP
    uint64_t timestamp_EXECUTE = TIME_MS;
#endif
    // EXEC PHASE
    task2_exec_core1(&local_data);
    sleep_ms_core_1(TASK_2_EXEC_DELAY);
    // EXEC PHASE END
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_WRITE = TIME_MS;
#endif
    // WRITE PHASE
    memcpy(&data_out, &local_data.local_data_out, sizeof(data_out));
    // WRITE PHASE END
    sleep_ms_core_1(TASK_2_WRITE_DELAY);
    __UNLOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_PASS = TIME_MS;
    // printf("\nCORE_1, TASK_2\nRead: %lli, execute: %lli, write: %lli, pass: %lli\n", timestamp_READ, timestamp_EXECUTE, timestamp_WRITE, timestamp_PASS);
        append_printout_core_1(&printout_stack_core_1, (printout_t){.read = timestamp_READ, .exec = timestamp_EXECUTE, .write = timestamp_WRITE, .pass = timestamp_PASS, .core = 1, .task = 2});
#endif
#ifdef DEBUG
    printf("core_1 task_2: Return value: %d (should be 0)\n", data_out.complete);
#endif
}