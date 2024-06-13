

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
    unsigned int a[ TASK_5_PARAM ];
} data_in_t;

typedef struct 
{
    unsigned int a[ TASK_5_PARAM ];   
    int return_result;
} data_out_t;

typedef struct 
{
    data_in_t local_data_in;
    data_out_t local_data_out;
} local_data_t;

extern printout_stack_t printout_stack_core_0;
extern printout_stack_t printout_stack_core_1;

static __scratch_y("task2") data_in_t data_in;

void task5_init()
{
    for (int i = 0; i < TASK_5_PARAM; i++)
    {
        data_in.a[TASK_5_PARAM - i] = i;
    }
}

static __scratch_y("task2") data_out_t data_out;


void __core_0_code("task5")(task5_exec_core0)(local_data_t *local_data)
{
    unsigned int insertsort_a[ TASK_5_PARAM ];
    int insertsort_iters_i, insertsort_min_i, insertsort_max_i;
    int insertsort_iters_a, insertsort_min_a, insertsort_max_a;

    insertsort_iters_i = 0;
    insertsort_min_i = 100000;
    insertsort_max_i = 0;
    insertsort_iters_a = 0;
    insertsort_min_a = 100000;
    insertsort_max_a = 0;

    /* Initialize routine */
    register volatile int k;
    for ( k = 0; k < TASK_5_PARAM; k++ )
        insertsort_a[ k ] = local_data->local_data_in.a[ k ];

    /* Main entrypoint */
    int  i, j, temp;
    i = 2;

    insertsort_iters_i = 0;

    while ( i <= 10 ) {

        insertsort_iters_i++;

        j = i;

        insertsort_iters_a = 0;

        while ( insertsort_a[ j ] < insertsort_a[ j - 1 ] ) {
        insertsort_iters_a++;

        temp = insertsort_a[ j ];
        insertsort_a[ j ] = insertsort_a[ j - 1 ];
        insertsort_a[ j - 1 ] = temp;
        j--;
        }

        if ( insertsort_iters_a < insertsort_min_a )
        insertsort_min_a = insertsort_iters_a;
        if ( insertsort_iters_a > insertsort_max_a )
        insertsort_max_a = insertsort_iters_a;

        i++;
    }

    if ( insertsort_iters_i < insertsort_min_i )
        insertsort_min_i = insertsort_iters_i;
    if ( insertsort_iters_i > insertsort_max_i )
        insertsort_max_i = insertsort_iters_i;

    /* Return routine */
    int in, returnValue = 0;

    for ( in = 0; in < TASK_5_PARAM; in++ )
    {
        returnValue += insertsort_a[ in ];
        local_data->local_data_out.a[in] = insertsort_a[in];
    }

    local_data->local_data_out.return_result = returnValue ;


}

void __core_0_code("task5")(task5_core0)()
{
#ifdef DEBUG
    printf("task_5 entered \n");
#endif
    uint32_t irq;
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_READ = TIME_MS;
#endif
    // READ PHASE
    local_data_t local_data;
    memcpy(&local_data.local_data_in, &data_in, sizeof(data_in));
    sleep_ms_core_0(TASK_5_READ_DELAY);
    __UNLOCK_MEM__
#ifdef MEMMAP
    printf("task_5 Data in (should be SRAM_5) address, size: %p: %p\n", &data_in, sizeof(data_in));
    printf("task_5 Data out (should be SRAM_5) address, size: %p: %p\n", &data_out, sizeof(data_out));
    printf("task_5 Local data IN address: %p\n", &local_data.local_data_in);
    printf("task_5 Local data OUT address: %p\n", &local_data.local_data_out);
#endif
    // READ PHASE END
#ifdef TIMESTAMP
    uint64_t timestamp_EXECUTE = TIME_MS;
#endif
    // EXEC PHASE
    task5_exec_core0(&local_data);
    sleep_ms_core_0(TASK_5_EXEC_DELAY);
    // EXEC PHASE END
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_WRITE = TIME_MS;
#endif
    // WRITE PHASE
    memcpy(&data_out, &local_data.local_data_out, sizeof(data_out));
    // WRITE PHASE END
    sleep_ms_core_0(TASK_5_WRITE_DELAY);
    __UNLOCK_MEM__

#ifdef TIMESTAMP
    uint64_t timestamp_PASS = TIME_MS;
    // printf("\nCORE_0, TASK_5\nRead: %lli, execute: %lli, write: %lli, pass: %lli\n", timestamp_READ, timestamp_EXECUTE, timestamp_WRITE, timestamp_PASS);
    append_printout_core_0(&printout_stack_core_0, (printout_t){.read = timestamp_READ, .exec = timestamp_EXECUTE, .write = timestamp_WRITE, .pass = timestamp_PASS, .core = 0, .task = 5});
#endif
#ifdef DEBUG
    printf("core_0 task_5: Return value: %d (should be 55)\n", data_out.return_result);
#endif
}

void __core_0_code("task5")(task5_exec_core1)(local_data_t *local_data)
{
    unsigned int insertsort_a[ TASK_5_PARAM ];
    int insertsort_iters_i, insertsort_min_i, insertsort_max_i;
    int insertsort_iters_a, insertsort_min_a, insertsort_max_a;

    insertsort_iters_i = 0;
    insertsort_min_i = 100000;
    insertsort_max_i = 0;
    insertsort_iters_a = 0;
    insertsort_min_a = 100000;
    insertsort_max_a = 0;

    /* Initialize routine */
    register volatile int k;
    for ( k = 0; k < TASK_5_PARAM; k++ )
        insertsort_a[ k ] = local_data->local_data_in.a[ k ];

    /* Main entrypoint */
    int  i, j, temp;
    i = 2;

    insertsort_iters_i = 0;

    while ( i <= 10 ) {

        insertsort_iters_i++;

        j = i;

        insertsort_iters_a = 0;

        while ( insertsort_a[ j ] < insertsort_a[ j - 1 ] ) {
        insertsort_iters_a++;

        temp = insertsort_a[ j ];
        insertsort_a[ j ] = insertsort_a[ j - 1 ];
        insertsort_a[ j - 1 ] = temp;
        j--;
        }

        if ( insertsort_iters_a < insertsort_min_a )
        insertsort_min_a = insertsort_iters_a;
        if ( insertsort_iters_a > insertsort_max_a )
        insertsort_max_a = insertsort_iters_a;

        i++;
    }

    if ( insertsort_iters_i < insertsort_min_i )
        insertsort_min_i = insertsort_iters_i;
    if ( insertsort_iters_i > insertsort_max_i )
        insertsort_max_i = insertsort_iters_i;

    /* Return routine */
    int in, returnValue = 0;

    for ( in = 0; in < TASK_5_PARAM; in++ )
    {
        returnValue += insertsort_a[ in ];
        local_data->local_data_out.a[in] = insertsort_a[in];
    }

    local_data->local_data_out.return_result = returnValue ;


}

void __core_1_code("task5")(task5_core1)()
{
#ifdef DEBUG
    printf("task_5 entered \n");
#endif
    uint32_t irq;
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_READ = TIME_MS;
#endif
    // READ PHASE
    local_data_t local_data;
    memcpy(&local_data.local_data_in, &data_in, sizeof(data_in));
    sleep_ms_core_1(TASK_5_READ_DELAY);
    __UNLOCK_MEM__
#ifdef MEMMAP
    printf("task_5 Data in (should be SRAM_5) address, size: %p: %p\n", &data_in, sizeof(data_in));
    printf("task_5 Data out (should be SRAM_5) address, size: %p: %p\n", &data_out, sizeof(data_out));
    printf("task_5 Local data IN address: %p\n", &local_data.local_data_in);
    printf("task_5 Local data OUT address: %p\n", &local_data.local_data_out);
#endif
    // READ PHASE END
#ifdef TIMESTAMP
    uint64_t timestamp_EXECUTE = TIME_MS;
#endif
    // EXEC PHASE
    task5_exec_core1(&local_data);
    sleep_ms_core_1(TASK_5_EXEC_DELAY);
    // EXEC PHASE END
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_WRITE = TIME_MS;
#endif
    // WRITE PHASE
    memcpy(&data_out, &local_data.local_data_out, sizeof(data_out));
    // WRITE PHASE END
    sleep_ms_core_1(TASK_5_WRITE_DELAY);
    __UNLOCK_MEM__

#ifdef TIMESTAMP
    uint64_t timestamp_PASS = TIME_MS;
    // printf("\nCORE_1, TASK_5\nRead: %lli, execute: %lli, write: %lli, pass: %lli\n", timestamp_READ, timestamp_EXECUTE, timestamp_WRITE, timestamp_PASS);
        append_printout_core_1(&printout_stack_core_1, (printout_t){.read = timestamp_READ, .exec = timestamp_EXECUTE, .write = timestamp_WRITE, .pass = timestamp_PASS, .core = 1, .task = 5});
#endif
#ifdef DEBUG
    printf("core_1 task_5: Return value: %d (should be 55)\n", data_out.return_result);
#endif
}

