/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/c11.h"
#include "../mcfgthread/clock.h"
#include "../mcfgthread/thread.h"
#include <assert.h>
#include <stdio.h>

static mtx_t mutex;

int
main(void)
  {
    double now, delta;
    struct timespec timeout;
    int r;

    _MCF_thread_set_priority(__MCF_nullptr, _MCF_thread_priority_above_normal);

    r = mtx_init(&mutex, mtx_timed);
    assert(r == thrd_success);

    /* Round the time up.  */
    int64_t sleep_until = (int64_t) time(__MCF_nullptr) * 1000 + 2000;
    _MCF_sleep(&sleep_until);

    now = _MCF_perf_counter();
    timeout.tv_sec = time(__MCF_nullptr) + 1;
    timeout.tv_nsec = 100000000;
    r = mtx_timedlock(&mutex, &timeout);  /* lock it  */
    assert(r == thrd_success);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 100);

    now = _MCF_perf_counter();
    timeout.tv_sec = time(__MCF_nullptr) + 1;
    timeout.tv_nsec = 100000000;
    mutex.__rc_mtx[0].__owner[0] = 42;
    r = mtx_timedlock(&mutex, &timeout);
    assert(r == thrd_timedout);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1100 - 40);
    assert(delta <= 1200);
  }
