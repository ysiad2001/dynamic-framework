

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


struct binarysearch_DATA
{
    int key;
    int value;
};

typedef struct
{
    int keys[TASK_0_PARAM];
    int values[TASK_0_PARAM];
    int x;
} data_in_t;

typedef struct
{
    int binary_search_result;
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

static __scratch_y("task0") data_in_t data_in;
static __scratch_y("task0") data_out_t data_out;

void task0_init()
{
    data_in.x = 4;
    for(int i = 0;i<TASK_0_PARAM;i++)
    {
        data_in.keys[i] = i*2;
        data_in.values[i] = i*3;
    }
    data_in.values[2] = 4504;
}

void __core_0_code("task0")(task0_exec_core0)(local_data_t *local_data)
{
    struct binarysearch_DATA binarysearch_data[TASK_0_PARAM];
    int i;

    for (i = 0; i < TASK_0_PARAM; ++i)
    {
        binarysearch_data[i].key = local_data->local_data_in.keys[i];
        binarysearch_data[i].value = local_data->local_data_in.values[i];
    }

    int fvalue, mid, up, low;

    low = 0;
    up = 14;
    fvalue = -1;

    while (low <= up)
    {
        mid = (low + up) >> 1;

        if (binarysearch_data[mid].key == local_data->local_data_in.x)
        {
            up = low - 1;
            fvalue = binarysearch_data[mid].value;
        }
        else

            if (binarysearch_data[mid].key > local_data->local_data_in.x)
            up = mid - 1;
        else
            low = mid + 1;
    }
    local_data->local_data_out.binary_search_result = fvalue;
}

void __core_1_code("task0")(task0_exec_core1)(local_data_t *local_data)
{
    struct binarysearch_DATA binarysearch_data[TASK_0_PARAM];
    int i;

    for (i = 0; i < TASK_0_PARAM; ++i)
    {
        binarysearch_data[i].key = local_data->local_data_in.keys[i];
        binarysearch_data[i].value = local_data->local_data_in.values[i];
    }

    int fvalue, mid, up, low;

    low = 0;
    up = 14;
    fvalue = -1;

    while (low <= up)
    {
        mid = (low + up) >> 1;

        if (binarysearch_data[mid].key == local_data->local_data_in.x)
        {
            up = low - 1;
            fvalue = binarysearch_data[mid].value;
        }
        else if (binarysearch_data[mid].key > local_data->local_data_in.x)
            up = mid - 1;
        else
            low = mid + 1;
    }
    local_data->local_data_out.binary_search_result = fvalue;
}

void __core_0_code("task0")(task0_core0)()
{
    uint32_t irq;
#ifdef DEBUG
    printf("task_0 entered \n");
#endif
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_READ = TIME_MS;
#endif
    // READ PHASE
    local_data_t local_data;
    memcpy(&local_data.local_data_in, &data_in, sizeof(data_in));
    sleep_ms_core_0(TASK_0_READ_DELAY);
    __UNLOCK_MEM__
#ifdef MEMMAP
    printf("task_0 Data in (should be SRAM_5) address, size: %p: %p\n", &data_in, sizeof(data_in));
    printf("task_0 Data out (should be SRAM_5) address, size: %p: %p\n", &data_out, sizeof(data_out));
    printf("task_0 Local data IN address: %p\n", &local_data.local_data_in);
    printf("task_0 Local data OUT address: %p\n", &local_data.local_data_out);
#endif
    // READ PHASE END
#ifdef TIMESTAMP
    uint64_t timestamp_EXECUTE = TIME_MS;
#endif
    // EXEC PHASE
    task0_exec_core0(&local_data);
    sleep_ms_core_0(TASK_0_EXEC_DELAY);
    // EXEC PHASE END
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_WRITE = TIME_MS;
#endif
    // WRITE PHASE
    memcpy(&data_out, &local_data.local_data_out, sizeof(data_out));
    // WRITE PHASE END
    sleep_ms_core_0(TASK_0_WRITE_DELAY);
    __UNLOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_PASS = TIME_MS;
    // printf("\nCORE_0, TASK_0\nRead: %lli, execute: %lli, write: %lli, pass: %lli\n", timestamp_READ, timestamp_EXECUTE, timestamp_WRITE, timestamp_PASS);
    append_printout_core_0(&printout_stack_core_0, (printout_t){.read = timestamp_READ, .exec = timestamp_EXECUTE, .write = timestamp_WRITE, .pass = timestamp_PASS, .core = 0, .task = 0});
#endif
#ifdef DEBUG
    printf("core_0 task_0: Return value: %d (should be 4504)\n", data_out.binary_search_result);
#endif
}

void __core_1_code("task0")(task0_core1)()
{
    uint32_t irq;
#ifdef DEBUG
    printf("task_0 entered \n");
#endif
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_READ = TIME_MS;
#endif
    // READ PHASE
    local_data_t local_data;
    memcpy(&local_data.local_data_in, &data_in, sizeof(data_in));
    sleep_ms_core_1(TASK_0_READ_DELAY);
    __UNLOCK_MEM__
#ifdef MEMMAP
    printf("task_0 Data in (should be SRAM_5) address, size: %p: %p\n", &data_in, sizeof(data_in));
    printf("task_0 Data out (should be SRAM_5) address, size: %p: %p\n", &data_out, sizeof(data_out));
    printf("task_0 Local data IN address: %p\n", &local_data.local_data_in);
    printf("task_0 Local data OUT address: %p\n", &local_data.local_data_out);
#endif
    // READ PHASE END
#ifdef TIMESTAMP
    uint64_t timestamp_EXECUTE = TIME_MS;
#endif
    // EXEC PHASE
    task0_exec_core1(&local_data);
    sleep_ms_core_1(TASK_0_EXEC_DELAY);
    // EXEC PHASE END
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_WRITE = TIME_MS;
#endif
    // WRITE PHASE
    memcpy(&data_out, &local_data.local_data_out, sizeof(data_out));
    sleep_ms_core_1(TASK_0_WRITE_DELAY);
    // WRITE PHASE END
    __UNLOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_PASS = TIME_MS;
    // printf("\nCORE_1, TASK_0\nRead: %lli, execute: %lli, write: %lli, pass: %lli\n", timestamp_READ, timestamp_EXECUTE, timestamp_WRITE, timestamp_PASS);
        append_printout_core_1(&printout_stack_core_1, (printout_t){.read = timestamp_READ, .exec = timestamp_EXECUTE, .write = timestamp_WRITE, .pass = timestamp_PASS, .core = 1, .task = 0});
#endif
#ifdef DEBUG
    printf("core_1 task_0: Return value: %d (should be 4504)\n", data_out.binary_search_result);
#endif
}

