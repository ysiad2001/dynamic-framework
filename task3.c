

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

#define DCTSIZE 8
#define DESCALE(x, n) (((x) + (((int)1) << ((n) - 1))) >> (n))

#define CONST_BITS 13
#define PASS1_BITS 2

#define FIX_0_298631336 ((int)2446)  /* FIX(0.298631336) */
#define FIX_0_390180644 ((int)3196)  /* FIX(0.390180644) */
#define FIX_0_541196100 ((int)4433)  /* FIX(0.541196100) */
#define FIX_0_765366865 ((int)6270)  /* FIX(0.765366865) */
#define FIX_0_899976223 ((int)7373)  /* FIX(0.899976223) */
#define FIX_1_175875602 ((int)9633)  /* FIX(1.175875602) */
#define FIX_1_501321110 ((int)12299) /* FIX(1.501321110) */
#define FIX_1_847759065 ((int)15137) /* FIX(1.847759065) */
#define FIX_1_961570560 ((int)16069) /* FIX(1.961570560) */
#define FIX_2_053119869 ((int)16819) /* FIX(2.053119869) */
#define FIX_2_562915447 ((int)20995) /* FIX(2.562915447) */
#define FIX_3_072711026 ((int)25172) /* FIX(3.072711026) */

typedef struct
{
    int checksum;
    int initial_coefficients[64];
} data_in_t;

typedef struct
{
    int transformation_result;
    int transformed_coefficients[64];
} data_out_t;

typedef struct
{
    data_in_t local_data_in;
    data_out_t local_data_out;
} local_data_t;

extern printout_stack_t printout_stack_core_0;
extern printout_stack_t printout_stack_core_1;

void task3_init()
{
    return;
}

static __scratch_y("task3") data_in_t data_in = {
    .initial_coefficients = {214, 28543, 60805, 26341, 30079, 2953, 65155, 15076, 39139, 28303, 28885, 40756, 46759, 58738, 13570, 35446, 61414, 41803, 54940, 32716, 25999, 50128, 48070, 36496, 4459, 3313, 47500, 26221, 14119, 42928, 7960, 10201, 46114, 38488, 7255, 47506, 27019, 54718, 3190, 31141, 13129, 42328, 59230, 13471, 22279, 14113, 42130, 32896, 49939, 22933, 35560, 11041, 26764, 20803, 14410, 16096, 43729, 48958, 23530, 49426, 20239, 4933, 820, 43606},
    .checksum = 1668124};
static __scratch_y("task3") data_out_t data_out;

void __core_0_code("task3")(jfdctint_jpeg_fdct_islow_core0)(int jfdctint_data[])
{
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    int tmp10, tmp11, tmp12, tmp13;
    int z1, z2, z3, z4, z5;
    int *dataptr;
    int ctr;

    /* Pass 1: process rows. */
    /* Note results are scaled up by sqrt(8) compared to a true DCT; */
    /* furthermore, we scale the results by 2**PASS1_BITS. */

    dataptr = jfdctint_data;
    for (ctr = DCTSIZE - 1; ctr >= 0; ctr--)
    {

        tmp0 = dataptr[0] + dataptr[7];
        tmp7 = dataptr[0] - dataptr[7];
        tmp1 = dataptr[1] + dataptr[6];
        tmp6 = dataptr[1] - dataptr[6];
        tmp2 = dataptr[2] + dataptr[5];
        tmp5 = dataptr[2] - dataptr[5];
        tmp3 = dataptr[3] + dataptr[4];
        tmp4 = dataptr[3] - dataptr[4];

        tmp10 = tmp0 + tmp3;
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[0] = (int)((tmp10 + tmp11) << PASS1_BITS);
        dataptr[4] = (int)((tmp10 - tmp11) << PASS1_BITS);

        z1 = (tmp12 + tmp13) * FIX_0_541196100;
        dataptr[2] = (int)DESCALE(z1 + tmp13 * FIX_0_765366865,
                                  CONST_BITS - PASS1_BITS);
        dataptr[6] = (int)DESCALE(z1 + tmp12 * (-FIX_1_847759065),
                                  CONST_BITS - PASS1_BITS);

        z1 = tmp4 + tmp7;
        z2 = tmp5 + tmp6;
        z3 = tmp4 + tmp6;
        z4 = tmp5 + tmp7;
        z5 = (z3 + z4) * FIX_1_175875602; /* sqrt(2) * c3 */

        tmp4 = tmp4 * FIX_0_298631336; /* sqrt(2) * (-c1+c3+c5-c7) */
        tmp5 = tmp5 * FIX_2_053119869; /* sqrt(2) * ( c1+c3-c5+c7) */
        tmp6 = tmp6 * FIX_3_072711026; /* sqrt(2) * ( c1+c3+c5-c7) */
        tmp7 = tmp7 * FIX_1_501321110; /* sqrt(2) * ( c1+c3-c5-c7) */
        z1 = z1 * (-FIX_0_899976223);  /* sqrt(2) * (c7-c3) */
        z2 = z2 * (-FIX_2_562915447);  /* sqrt(2) * (-c1-c3) */
        z3 = z3 * (-FIX_1_961570560);  /* sqrt(2) * (-c3-c5) */
        z4 = z4 * (-FIX_0_390180644);  /* sqrt(2) * (c5-c3) */

        z3 += z5;
        z4 += z5;

        dataptr[7] = (int)DESCALE(tmp4 + z1 + z3, CONST_BITS - PASS1_BITS);
        dataptr[5] = (int)DESCALE(tmp5 + z2 + z4, CONST_BITS - PASS1_BITS);
        dataptr[3] = (int)DESCALE(tmp6 + z2 + z3, CONST_BITS - PASS1_BITS);
        dataptr[1] = (int)DESCALE(tmp7 + z1 + z4, CONST_BITS - PASS1_BITS);

        dataptr += DCTSIZE; /* advance pointer to next row */
    }

    dataptr = jfdctint_data;
    for (ctr = DCTSIZE - 1; ctr >= 0; ctr--)
    {
        tmp0 = dataptr[DCTSIZE * 0] + dataptr[DCTSIZE * 7];
        tmp7 = dataptr[DCTSIZE * 0] - dataptr[DCTSIZE * 7];
        tmp1 = dataptr[DCTSIZE * 1] + dataptr[DCTSIZE * 6];
        tmp6 = dataptr[DCTSIZE * 1] - dataptr[DCTSIZE * 6];
        tmp2 = dataptr[DCTSIZE * 2] + dataptr[DCTSIZE * 5];
        tmp5 = dataptr[DCTSIZE * 2] - dataptr[DCTSIZE * 5];
        tmp3 = dataptr[DCTSIZE * 3] + dataptr[DCTSIZE * 4];
        tmp4 = dataptr[DCTSIZE * 3] - dataptr[DCTSIZE * 4];

        tmp10 = tmp0 + tmp3;
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[DCTSIZE * 0] = (int)DESCALE(tmp10 + tmp11, PASS1_BITS);
        dataptr[DCTSIZE * 4] = (int)DESCALE(tmp10 - tmp11, PASS1_BITS);

        z1 = (tmp12 + tmp13) * FIX_0_541196100;
        dataptr[DCTSIZE * 2] = (int)DESCALE(z1 + tmp13 * FIX_0_765366865,
                                            CONST_BITS + PASS1_BITS);
        dataptr[DCTSIZE * 6] = (int)DESCALE(z1 + tmp12 * (-FIX_1_847759065),
                                            CONST_BITS + PASS1_BITS);

        z1 = tmp4 + tmp7;
        z2 = tmp5 + tmp6;
        z3 = tmp4 + tmp6;
        z4 = tmp5 + tmp7;
        z5 = (z3 + z4) * FIX_1_175875602; /* sqrt(2) * c3 */

        tmp4 = tmp4 * FIX_0_298631336; /* sqrt(2) * (-c1+c3+c5-c7) */
        tmp5 = tmp5 * FIX_2_053119869; /* sqrt(2) * ( c1+c3-c5+c7) */
        tmp6 = tmp6 * FIX_3_072711026; /* sqrt(2) * ( c1+c3+c5-c7) */
        tmp7 = tmp7 * FIX_1_501321110; /* sqrt(2) * ( c1+c3-c5-c7) */
        z1 = z1 * (-FIX_0_899976223);  /* sqrt(2) * (c7-c3) */
        z2 = z2 * (-FIX_2_562915447);  /* sqrt(2) * (-c1-c3) */
        z3 = z3 * (-FIX_1_961570560);  /* sqrt(2) * (-c3-c5) */
        z4 = z4 * (-FIX_0_390180644);  /* sqrt(2) * (c5-c3) */

        z3 += z5;
        z4 += z5;

        dataptr[DCTSIZE * 7] = (int)DESCALE(tmp4 + z1 + z3,
                                            CONST_BITS + PASS1_BITS);
        dataptr[DCTSIZE * 5] = (int)DESCALE(tmp5 + z2 + z4,
                                            CONST_BITS + PASS1_BITS);
        dataptr[DCTSIZE * 3] = (int)DESCALE(tmp6 + z2 + z3,
                                            CONST_BITS + PASS1_BITS);
        dataptr[DCTSIZE * 1] = (int)DESCALE(tmp7 + z1 + z4,
                                            CONST_BITS + PASS1_BITS);

        dataptr++; /* advance pointer to next column */
    }
}

/* Place an attribute to note that this is an exec function */
void __core_0_code("task3")(task3_exec_core0)(local_data_t *local_data)
{
    /* Init routine modified since a modulo operator was used, data simply copy pasted to local data */
    int jfdctint_data[64];

    for (int i = 0; i < 64; i++)
    {
        jfdctint_data[i] = local_data->local_data_in.initial_coefficients[i];
    }

    /* Main entrypoint */

    jfdctint_jpeg_fdct_islow_core0(jfdctint_data);

    int checksum = 0;
    int i;
    for (i = 0; i < 64; ++i)
    {
        checksum += jfdctint_data[i];
        local_data->local_data_out.transformed_coefficients[i] = jfdctint_data[i];
    }
    local_data->local_data_out.transformation_result = ((checksum == local_data->local_data_in.checksum) ? 0 : -1);
}

void __core_0_code("task3")(task3_core0)()
{
    uint32_t irq;
#ifdef DEBUG
    printf("task_3 entered \n");
#endif
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_READ = TIME_MS;
#endif
    // READ PHASE
    local_data_t local_data;
    memcpy(&local_data.local_data_in, &data_in, sizeof(data_in));
    sleep_ms_core_0(TASK_3_READ_DELAY);
    __UNLOCK_MEM__
#ifdef MEMMAP
    printf("task_3 Data in (should be SRAM_5) address, size: %p: %p\n", &data_in, sizeof(data_in));
    printf("task_3 Data out (should be SRAM_5) address, size: %p: %p\n", &data_out, sizeof(data_out));
    printf("task_3 Local data IN address: %p\n", &local_data.local_data_in);
    printf("task_3 Local data OUT address: %p\n", &local_data.local_data_out);
#endif
    // READ PHASE END
#ifdef TIMESTAMP
    uint64_t timestamp_EXECUTE = TIME_MS;
#endif
    // EXEC PHASE
    task3_exec_core0(&local_data);
    sleep_ms_core_0(TASK_3_EXEC_DELAY);
    // EXEC PHASE END
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_WRITE = TIME_MS;
#endif
    // WRITE PHASE
    memcpy(&data_out, &local_data.local_data_out, sizeof(data_out));
    // WRITE PHASE END
    sleep_ms_core_0(TASK_3_WRITE_DELAY);
    __UNLOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_PASS = TIME_MS;
    // printf("\nCORE_0, TASK_3\nRead: %lli, execute: %lli, write: %lli, pass: %lli\n", timestamp_READ, timestamp_EXECUTE, timestamp_WRITE, timestamp_PASS);
    append_printout_core_0(&printout_stack_core_0, (printout_t){.read = timestamp_READ, .exec = timestamp_EXECUTE, .write = timestamp_WRITE, .pass = timestamp_PASS, .core = 0, .task = 3});
#endif
#ifdef DEBUG
    printf("core_0 task_3: Return value: %d (should be 0)\n", data_out.transformation_result);
#endif
}

void __core_1_code("task3")(jfdctint_jpeg_fdct_islow_core1)(int jfdctint_data[])
{
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    int tmp10, tmp11, tmp12, tmp13;
    int z1, z2, z3, z4, z5;
    int *dataptr;
    int ctr;

    /* Pass 1: process rows. */
    /* Note results are scaled up by sqrt(8) compared to a true DCT; */
    /* furthermore, we scale the results by 2**PASS1_BITS. */

    dataptr = jfdctint_data;
    for (ctr = DCTSIZE - 1; ctr >= 0; ctr--)
    {

        tmp0 = dataptr[0] + dataptr[7];
        tmp7 = dataptr[0] - dataptr[7];
        tmp1 = dataptr[1] + dataptr[6];
        tmp6 = dataptr[1] - dataptr[6];
        tmp2 = dataptr[2] + dataptr[5];
        tmp5 = dataptr[2] - dataptr[5];
        tmp3 = dataptr[3] + dataptr[4];
        tmp4 = dataptr[3] - dataptr[4];

        tmp10 = tmp0 + tmp3;
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[0] = (int)((tmp10 + tmp11) << PASS1_BITS);
        dataptr[4] = (int)((tmp10 - tmp11) << PASS1_BITS);

        z1 = (tmp12 + tmp13) * FIX_0_541196100;
        dataptr[2] = (int)DESCALE(z1 + tmp13 * FIX_0_765366865,
                                  CONST_BITS - PASS1_BITS);
        dataptr[6] = (int)DESCALE(z1 + tmp12 * (-FIX_1_847759065),
                                  CONST_BITS - PASS1_BITS);

        z1 = tmp4 + tmp7;
        z2 = tmp5 + tmp6;
        z3 = tmp4 + tmp6;
        z4 = tmp5 + tmp7;
        z5 = (z3 + z4) * FIX_1_175875602; /* sqrt(2) * c3 */

        tmp4 = tmp4 * FIX_0_298631336; /* sqrt(2) * (-c1+c3+c5-c7) */
        tmp5 = tmp5 * FIX_2_053119869; /* sqrt(2) * ( c1+c3-c5+c7) */
        tmp6 = tmp6 * FIX_3_072711026; /* sqrt(2) * ( c1+c3+c5-c7) */
        tmp7 = tmp7 * FIX_1_501321110; /* sqrt(2) * ( c1+c3-c5-c7) */
        z1 = z1 * (-FIX_0_899976223);  /* sqrt(2) * (c7-c3) */
        z2 = z2 * (-FIX_2_562915447);  /* sqrt(2) * (-c1-c3) */
        z3 = z3 * (-FIX_1_961570560);  /* sqrt(2) * (-c3-c5) */
        z4 = z4 * (-FIX_0_390180644);  /* sqrt(2) * (c5-c3) */

        z3 += z5;
        z4 += z5;

        dataptr[7] = (int)DESCALE(tmp4 + z1 + z3, CONST_BITS - PASS1_BITS);
        dataptr[5] = (int)DESCALE(tmp5 + z2 + z4, CONST_BITS - PASS1_BITS);
        dataptr[3] = (int)DESCALE(tmp6 + z2 + z3, CONST_BITS - PASS1_BITS);
        dataptr[1] = (int)DESCALE(tmp7 + z1 + z4, CONST_BITS - PASS1_BITS);

        dataptr += DCTSIZE; /* advance pointer to next row */
    }

    dataptr = jfdctint_data;
    for (ctr = DCTSIZE - 1; ctr >= 0; ctr--)
    {
        tmp0 = dataptr[DCTSIZE * 0] + dataptr[DCTSIZE * 7];
        tmp7 = dataptr[DCTSIZE * 0] - dataptr[DCTSIZE * 7];
        tmp1 = dataptr[DCTSIZE * 1] + dataptr[DCTSIZE * 6];
        tmp6 = dataptr[DCTSIZE * 1] - dataptr[DCTSIZE * 6];
        tmp2 = dataptr[DCTSIZE * 2] + dataptr[DCTSIZE * 5];
        tmp5 = dataptr[DCTSIZE * 2] - dataptr[DCTSIZE * 5];
        tmp3 = dataptr[DCTSIZE * 3] + dataptr[DCTSIZE * 4];
        tmp4 = dataptr[DCTSIZE * 3] - dataptr[DCTSIZE * 4];

        tmp10 = tmp0 + tmp3;
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[DCTSIZE * 0] = (int)DESCALE(tmp10 + tmp11, PASS1_BITS);
        dataptr[DCTSIZE * 4] = (int)DESCALE(tmp10 - tmp11, PASS1_BITS);

        z1 = (tmp12 + tmp13) * FIX_0_541196100;
        dataptr[DCTSIZE * 2] = (int)DESCALE(z1 + tmp13 * FIX_0_765366865,
                                            CONST_BITS + PASS1_BITS);
        dataptr[DCTSIZE * 6] = (int)DESCALE(z1 + tmp12 * (-FIX_1_847759065),
                                            CONST_BITS + PASS1_BITS);

        z1 = tmp4 + tmp7;
        z2 = tmp5 + tmp6;
        z3 = tmp4 + tmp6;
        z4 = tmp5 + tmp7;
        z5 = (z3 + z4) * FIX_1_175875602; /* sqrt(2) * c3 */

        tmp4 = tmp4 * FIX_0_298631336; /* sqrt(2) * (-c1+c3+c5-c7) */
        tmp5 = tmp5 * FIX_2_053119869; /* sqrt(2) * ( c1+c3-c5+c7) */
        tmp6 = tmp6 * FIX_3_072711026; /* sqrt(2) * ( c1+c3+c5-c7) */
        tmp7 = tmp7 * FIX_1_501321110; /* sqrt(2) * ( c1+c3-c5-c7) */
        z1 = z1 * (-FIX_0_899976223);  /* sqrt(2) * (c7-c3) */
        z2 = z2 * (-FIX_2_562915447);  /* sqrt(2) * (-c1-c3) */
        z3 = z3 * (-FIX_1_961570560);  /* sqrt(2) * (-c3-c5) */
        z4 = z4 * (-FIX_0_390180644);  /* sqrt(2) * (c5-c3) */

        z3 += z5;
        z4 += z5;

        dataptr[DCTSIZE * 7] = (int)DESCALE(tmp4 + z1 + z3,
                                            CONST_BITS + PASS1_BITS);
        dataptr[DCTSIZE * 5] = (int)DESCALE(tmp5 + z2 + z4,
                                            CONST_BITS + PASS1_BITS);
        dataptr[DCTSIZE * 3] = (int)DESCALE(tmp6 + z2 + z3,
                                            CONST_BITS + PASS1_BITS);
        dataptr[DCTSIZE * 1] = (int)DESCALE(tmp7 + z1 + z4,
                                            CONST_BITS + PASS1_BITS);

        dataptr++; /* advance pointer to next column */
    }
}

/* Place an attribute to note that this is an exec function */
void __core_1_code("task3")(task3_exec_core1)(local_data_t *local_data)
{
    /* Init routine modified since a modulo operator was used, data simply copy pasted to local data */
    int jfdctint_data[64];

    for (int i = 0; i < 64; i++)
    {
        jfdctint_data[i] = local_data->local_data_in.initial_coefficients[i];
    }

    /* Main entrypoint */

    jfdctint_jpeg_fdct_islow_core1(jfdctint_data);

    int checksum = 0;
    int i;
    for (i = 0; i < 64; ++i)
    {
        checksum += jfdctint_data[i];
        local_data->local_data_out.transformed_coefficients[i] = jfdctint_data[i];
    }
    local_data->local_data_out.transformation_result = ((checksum == local_data->local_data_in.checksum) ? 0 : -1);
}

void __core_1_code("task3")(task3_core1)()
{
#ifdef DEBUG
    printf("task_3 entered \n");
#endif
    uint32_t irq;
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_READ = TIME_MS;
#endif
    // READ PHASE
    local_data_t local_data;
    memcpy(&local_data.local_data_in, &data_in, sizeof(data_in));
    sleep_ms_core_1(TASK_3_READ_DELAY);
    __UNLOCK_MEM__
#ifdef MEMMAP
    printf("task_3 Data in (should be SRAM_5) address, size: %p: %p\n", &data_in, sizeof(data_in));
    printf("task_3 Data out (should be SRAM_5) address, size: %p: %p\n", &data_out, sizeof(data_out));
    printf("task_3 Local data IN address: %p\n", &local_data.local_data_in);
    printf("task_3 Local data OUT address: %p\n", &local_data.local_data_out);
#endif
    // READ PHASE END
#ifdef TIMESTAMP
    uint64_t timestamp_EXECUTE = TIME_MS;
#endif
    // EXEC PHASE
    task3_exec_core1(&local_data);
    sleep_ms_core_1(TASK_3_EXEC_DELAY);
    // EXEC PHASE END
    __LOCK_MEM__
#ifdef TIMESTAMP
    uint64_t timestamp_WRITE = TIME_MS;
#endif
    // WRITE PHASE
    memcpy(&data_out, &local_data.local_data_out, sizeof(data_out));
    // WRITE PHASE END
    sleep_ms_core_1(TASK_3_WRITE_DELAY);
    __UNLOCK_MEM__

#ifdef TIMESTAMP
    uint64_t timestamp_PASS = TIME_MS;
    // printf("\nCORE_1, TASK_3\nRead: %lli, execute: %lli, write: %lli, pass: %lli\n", timestamp_READ, timestamp_EXECUTE, timestamp_WRITE, timestamp_PASS);
        append_printout_core_1(&printout_stack_core_1, (printout_t){.read = timestamp_READ, .exec = timestamp_EXECUTE, .write = timestamp_WRITE, .pass = timestamp_PASS, .core = 1, .task = 3});
#endif
#ifdef DEBUG
    printf("core_1 task_3: Return value: %d (should be 0)\n", data_out.transformation_result);
#endif
}
