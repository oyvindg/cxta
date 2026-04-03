#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

/*
 * cxta rolling tests:
 * - rolling sum/sumsq update
 * - rolling mean/variance
 */
void cxta_test_rolling(void) {
    cxta_ts_rolling_state st;
    cxta_ts_rolling_reset(&st);
    cxta_ts_rolling_add(&st, 1.0);
    cxta_ts_rolling_add(&st, 2.0);
    cxta_ts_rolling_add(&st, 3.0);
    assert(fabs(cxta_ts_rolling_mean(&st) - 2.0) < 1e-12);
    assert(fabs(cxta_ts_rolling_var(&st) - (2.0 / 3.0)) < 1e-12);
    cxta_ts_rolling_remove(&st, 1.0);
    assert(fabs(cxta_ts_rolling_mean(&st) - 2.5) < 1e-12);
    printf("  ✓ test_rolling\n");
}
