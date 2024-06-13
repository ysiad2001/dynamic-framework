

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
    int input;
} data_in_t;

typedef struct
{
    int output;

} data_out_t;

typedef struct
{
    data_in_t local_data_in;
    data_out_t local_data_out;
} local_data_t;

extern printout_stack_t printout_stack_core_0;
extern printout_stack_t printout_stack_core_1;

static __scratch_y("task4") data_in_t data_in;

static __scratch_y("task4") data_out_t data_out;

void task4_init()
{
    data_in.input = TASK_4_PARAM;
}

int __core_0_code("task4") recursion_fib_core0(int i)
{
    if (i == 0)
        return 1;
    if (i == 1)
        return 1;
    return recursion_fib_core0(i - 1) + recursion_fib_core0(i - 2);
}

void __core_0_code("task4")(task4_exec_core0)(local_data_t *local_data)
{
    int output = recursion_fib_core0(local_data->local_data_in.input);
    local_data->local_data_out.output = output;
}

void __core_0_code("task4")(task4_core0)()
{
#ifdef DEBUG
    printf("task_4 entered \n");
#endif
    uint32_t irq;
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_READ = TIME_MS;
#endif
    // READ PHASE
    local_data_t local_data;
    memcpy(&local_data.local_data_in, &data_in, sizeof(data_in));
    sleep_ms_core_0(TASK_4_READ_DELAY);
    __UNLOCK_MEM__
#ifdef MEMMAP
    printf("task_4 Data in (should be SRAM_5) address, size: %p: %p\n", &data_in, sizeof(data_in));
    printf("task_4 Data out (should be SRAM_5) address, size: %p: %p\n", &data_out, sizeof(data_out));
    printf("task_4 Local data IN address: %p\n", &local_data.local_data_in);
    printf("task_4 Local data OUT address: %p\n", &local_data.local_data_out);
#endif
    // READ PHASE END
#ifdef TIMESTAMP
    uint64_t timestamp_EXECUTE = TIME_MS;
#endif
    // EXEC PHASE
    task4_exec_core0(&local_data);
    sleep_ms_core_0(TASK_4_EXEC_DELAY);
    // EXEC PHASE END
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_WRITE = TIME_MS;
#endif
    // WRITE PHASE
    memcpy(&data_out, &local_data.local_data_out, sizeof(data_out));
    // WRITE PHASE END
    sleep_ms_core_0(TASK_4_WRITE_DELAY);
    __UNLOCK_MEM__

#ifdef TIMESTAMP
    uint64_t timestamp_PASS = TIME_MS;
    // printf("\nCORE_0, TASK_4\nRead: %lli, execute: %lli, write: %lli, pass: %lli\n", timestamp_READ, timestamp_EXECUTE, timestamp_WRITE, timestamp_PASS);
    append_printout_core_0(&printout_stack_core_0, (printout_t){.read = timestamp_READ, .exec = timestamp_EXECUTE, .write = timestamp_WRITE, .pass = timestamp_PASS, .core = 0, .task = 4});
#endif
#ifdef DEBUG
    printf("core_0 task_4: Return value: %d (should be 987)\n", data_out.output);
#endif
}

int __core_1_code("task4") recursion_fib_core1(int i)
{
    if (i == 0)
        return 1;
    if (i == 1)
        return 1;
    return recursion_fib_core1(i - 1) + recursion_fib_core1(i - 2);
}

void __core_1_code("task4")(task4_exec_core1)(local_data_t *local_data)
{
    int output = recursion_fib_core1(local_data->local_data_in.input);
    local_data->local_data_out.output = output;
}

void __core_1_code("task4")(task4_core1)()
{
#ifdef DEBUG
    printf("task_4 entered \n");
#endif
    uint32_t irq;
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_READ = TIME_MS;
#endif
    // READ PHASE
    local_data_t local_data;
    memcpy(&local_data.local_data_in, &data_in, sizeof(data_in));
    sleep_ms_core_1(TASK_4_READ_DELAY);
    __UNLOCK_MEM__
#ifdef MEMMAP
    printf("task_4 Data in (should be SRAM_5) address, size: %p: %p\n", &data_in, sizeof(data_in));
    printf("task_4 Data out (should be SRAM_5) address, size: %p: %p\n", &data_out, sizeof(data_out));
    printf("task_4 Local data IN address: %p\n", &local_data.local_data_in);
    printf("task_4 Local data OUT address: %p\n", &local_data.local_data_out);
#endif
    // READ PHASE END
#ifdef TIMESTAMP
    uint64_t timestamp_EXECUTE = TIME_MS;
#endif
    // EXEC PHASE
    task4_exec_core1(&local_data);
    sleep_ms_core_1(TASK_4_EXEC_DELAY);
    // EXEC PHASE END
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_WRITE = TIME_MS;
#endif
    // WRITE PHASE
    memcpy(&data_out, &local_data.local_data_out, sizeof(data_out));
    // WRITE PHASE END
    sleep_ms_core_1(TASK_4_WRITE_DELAY);
    __UNLOCK_MEM__

#ifdef TIMESTAMP
    uint64_t timestamp_PASS = TIME_MS;
    // printf("\nCORE_1, TASK_4\nRead: %lli, execute: %lli, write: %lli, pass: %lli\n", timestamp_READ, timestamp_EXECUTE, timestamp_WRITE, timestamp_PASS);
        append_printout_core_1(&printout_stack_core_1, (printout_t){.read = timestamp_READ, .exec = timestamp_EXECUTE, .write = timestamp_WRITE, .pass = timestamp_PASS, .core = 1, .task = 4});
#endif
#ifdef DEBUG
    printf("core_1 task_4: Return value: %d (should be 987)\n", data_out.output);
#endif
}
