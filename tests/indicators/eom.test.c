#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_eom(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 10.0, 8.0,  0.0, 0.0},
        {0, 0.0, 12.0, 8.0,  0.0, 1000.0},
    };
    const cxta_series_bar_view v0 = cxta_series_bar_view_make(bars, 2, 0);
    /* First bar → 0 */
    assert(fabs(cxta_eom(&v0)) < 1e-12);
    const cxta_series_bar_view v1 = cxta_series_bar_view_make(bars, 2, 1);
    /* midpoint_move = (12+8)/2 - (10+8)/2 = 10 - 9 = 1, range=4, vol=1000 → 4/1000 */
    assert(fabs(cxta_eom(&v1) - 4.0 / 1000.0) < 1e-12);
    printf("  ✓ test_eom\n");
}
