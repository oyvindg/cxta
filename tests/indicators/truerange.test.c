#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_truerange(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 10.0, 8.0, 9.0, 0.0},
        {0, 0.0, 12.0, 7.0, 11.0, 0.0},
    };
    /* First bar: H-L = 2 */
    const cxta_series_bar_view v0 = cxta_series_bar_view_make(bars, 2, 0);
    assert(fabs(cxta_truerange(&v0) - 2.0) < 1e-12);
    /* Second bar: max(12-7, 12-9, 9-7) = max(5,3,2) = 5 */
    const cxta_series_bar_view v1 = cxta_series_bar_view_make(bars, 2, 1);
    assert(fabs(cxta_truerange(&v1) - 5.0) < 1e-12);
    printf("  ✓ test_truerange\n");
}
