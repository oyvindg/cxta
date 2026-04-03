#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

/*
 * cxta core tests:
 * - period clamp
 * - EMA/Wilder steps
 * - true range
 */
void cxta_test_range(void) {
    const double tr = cxta_ts_true_range(12.0, 10.0, 11.5);
    assert(fabs(tr - 2.0) < 1e-12);
    printf("  ✓ test_true_range\n");
}
