

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
    uint32_t arr[TASK_1_PARAM];
} data_in_t;

typedef struct
{
    uint32_t sorted_arr[TASK_1_PARAM];
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

static __scratch_y("task1") data_in_t data_in;
static __scratch_y("task1") data_out_t data_out;

void task1_init()
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

#define bitonic_ASCENDING (1)
#define bitonic_DESCENDING (0)

void __core_0_code("task1") bitonic_compare_core0(uint32_t i, uint32_t j, uint32_t dir, uint32_t *arr)
{
    if (dir == (arr[i] > arr[j]))
    {
        uint32_t h = arr[i];
        arr[i] = arr[j];
        arr[j] = h;
    }
}

void __core_0_code("task1") bitonic_merge_core0(uint32_t lo, uint32_t cnt, uint32_t dir, uint32_t *arr)
{
    uint32_t k = cnt / 2;
    uint32_t i;
    for (i = lo; i < lo + k; i++)
        bitonic_compare_core0(i, i + k, dir, arr);

    if (k > 1)
    {
        bitonic_merge_core0(lo, k, dir, arr);
        bitonic_merge_core0(lo + k, k, dir, arr);
    }
}

void __core_0_code("task1") bitonic_sort_core0(uint32_t lo, uint32_t cnt, uint32_t dir, uint32_t *arr)
{
    uint32_t k = cnt;
    k /= 2;

    if (cnt > 1)
    {
        bitonic_sort_core0(lo, k, bitonic_ASCENDING, arr);
        bitonic_sort_core0(lo + k, k, bitonic_DESCENDING, arr);
    }

    bitonic_merge_core0(lo, cnt, dir, arr);

    return;
}

void __core_0_code("task1")(task1_exec_core0)(local_data_t *local_data)
{
    uint32_t arr[TASK_1_PARAM];

    for (int i = 0; i < TASK_1_PARAM; ++i)
    {
        arr[i] = local_data->local_data_in.arr[i];
    }
    bitonic_sort_core0(0, TASK_1_PARAM, bitonic_ASCENDING, arr);

    for (int i = 0; i < TASK_1_PARAM; ++i)
    {
        local_data->local_data_out.sorted_arr[i] = arr[i];
    }
    local_data->local_data_out.complete = (local_data->local_data_out.sorted_arr[14]);
}

void __core_0_code("task1")(task1_core0)()
{
    uint32_t irq;
#ifdef DEBUG
    printf("task_1 entered \n");
#endif
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_READ = TIME_MS;
#endif
    // READ PHASE
    local_data_t local_data;
    memcpy(&local_data.local_data_in, &data_in, sizeof(data_in));
    sleep_ms_core_0(TASK_1_READ_DELAY);
    __UNLOCK_MEM__
#ifdef MEMMAP
    printf("task_1 Data in (should be SRAM_5) address, size: %p: %p\n", &data_in, sizeof(data_in));
    printf("task_1 Data out (should be SRAM_5) address, size: %p: %p\n", &data_out, sizeof(data_out));
    printf("task_1 Local data IN address: %p\n", &local_data.local_data_in);
    printf("task_1 Local data OUT address: %p\n", &local_data.local_data_out);
#endif
    // READ PHASE END
#ifdef TIMESTAMP
    uint64_t timestamp_EXECUTE = TIME_MS;
#endif
    // EXEC PHASE
    task1_exec_core0(&local_data);
    sleep_ms_core_0(TASK_1_EXEC_DELAY);
    // EXEC PHASE END
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_WRITE = TIME_MS;
#endif
    // WRITE PHASE
    memcpy(&data_out, &local_data.local_data_out, sizeof(data_out));
    // WRITE PHASE END
    sleep_ms_core_0(TASK_1_WRITE_DELAY);
    __UNLOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_PASS = TIME_MS;
    // printf("\nCORE_0, TASK_1\nRead: %lli, execute: %lli, write: %lli, pass: %lli\n", timestamp_READ, timestamp_EXECUTE, timestamp_WRITE, timestamp_PASS);
    append_printout_core_0(&printout_stack_core_0, (printout_t){.read = timestamp_READ, .exec = timestamp_EXECUTE, .write = timestamp_WRITE, .pass = timestamp_PASS, .core = 0, .task = 1});
#endif
#ifdef DEBUG
    printf("core_0 task_1: Return value: %d (should be 66)\n", data_out.complete);
#endif
}

void __core_1_code("task1") bitonic_compare_core1(uint32_t i, uint32_t j, uint32_t dir, uint32_t *arr)
{
    if (dir == (arr[i] > arr[j]))
    {
        uint32_t h = arr[i];
        arr[i] = arr[j];
        arr[j] = h;
    }
}

void __core_1_code("task1") bitonic_merge_core1(uint32_t lo, uint32_t cnt, uint32_t dir, uint32_t *arr)
{
    uint32_t k = cnt / 2;
    uint32_t i;
    for (i = lo; i < lo + k; i++)
        bitonic_compare_core1(i, i + k, dir, arr);

    if (k > 1)
    {
        bitonic_merge_core1(lo, k, dir, arr);
        bitonic_merge_core1(lo + k, k, dir, arr);
    }
}

void __core_1_code("task1") bitonic_sort_core1(uint32_t lo, uint32_t cnt, uint32_t dir, uint32_t *arr)
{
    uint32_t k = cnt;
    k /= 2;

    if (cnt > 1)
    {
        bitonic_sort_core1(lo, k, bitonic_ASCENDING, arr);
        bitonic_sort_core1(lo + k, k, bitonic_DESCENDING, arr);
    }

    bitonic_merge_core1(lo, cnt, dir, arr);

    return;
}

void __core_1_code("task1")(task1_exec_core1)(local_data_t *local_data)
{
    uint32_t arr[TASK_1_PARAM];
    uint32_t i;

    for (i = 0; i < TASK_1_PARAM; ++i)
    {
        arr[i] = local_data->local_data_in.arr[i];
    }
    bitonic_sort_core1(0, TASK_1_PARAM, bitonic_ASCENDING, arr);
    for (i = 0; i < TASK_1_PARAM; ++i)
    {
        local_data->local_data_out.sorted_arr[i] = arr[i];
    }
    local_data->local_data_out.complete = ((local_data->local_data_out.sorted_arr[14]));
}

void __core_1_code("task1")(task1_core1)()
{
    uint32_t irq;
#ifdef DEBUG
    printf("task_1 entered \n");
#endif
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_READ = TIME_MS;
#endif
    // READ PHASE
    local_data_t local_data;
    memcpy(&local_data.local_data_in, &data_in, sizeof(data_in));
    sleep_ms_core_1(TASK_1_READ_DELAY);
    __UNLOCK_MEM__
#ifdef MEMMAP
    printf("task_1 Data in (should be SRAM_5) address, size: %p: %p\n", &data_in, sizeof(data_in));
    printf("task_1 Data out (should be SRAM_5) address, size: %p: %p\n", &data_out, sizeof(data_out));
    printf("task_1 Local data IN address: %p\n", &local_data.local_data_in);
    printf("task_1 Local data OUT address: %p\n", &local_data.local_data_out);
#endif
    // READ PHASE END
#ifdef TIMESTAMP
    uint64_t timestamp_EXECUTE = TIME_MS;
#endif
    // EXEC PHASE
    task1_exec_core1(&local_data);
    sleep_ms_core_1(TASK_1_EXEC_DELAY);
    // EXEC PHASE END
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_WRITE = TIME_MS;
#endif
    // WRITE PHASE
    memcpy(&data_out, &local_data.local_data_out, sizeof(data_out));
    // WRITE PHASE END
    sleep_ms_core_1(TASK_1_WRITE_DELAY);
    __UNLOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_PASS = TIME_MS;
    // printf("\nCORE_1, TASK_1\nRead: %lli, execute: %lli, write: %lli, pass: %lli\n", timestamp_READ, timestamp_EXECUTE, timestamp_WRITE, timestamp_PASS);
        append_printout_core_1(&printout_stack_core_1, (printout_t){.read = timestamp_READ, .exec = timestamp_EXECUTE, .write = timestamp_WRITE, .pass = timestamp_PASS, .core = 1, .task = 1});
#endif
#ifdef DEBUG
    printf("core_1 task_1: Return value: %d (should be 66)\n", data_out.complete);
#endif
}
