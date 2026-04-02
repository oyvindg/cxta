#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_rvol(void) {
    /* Constant series → zero log returns → rvol = 0 */
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 100.0, 0.0},
        {0, 0.0, 0.0, 0.0, 100.0, 0.0},
        {0, 0.0, 0.0, 0.0, 100.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    assert(fabs(cxta_rvol(&view, 2)) < 1e-12);
    /* Non-zero returns */
    const cxta_series_bar bars2[] = {
        {0, 0.0, 0.0, 0.0, 100.0, 0.0},
        {0, 0.0, 0.0, 0.0, 110.0, 0.0},
    };
    const cxta_series_bar_view view2 = cxta_series_bar_view_make(bars2, 2, 1);
    const double r = log(110.0 / 100.0);
    assert(fabs(cxta_rvol(&view2, 1) - fabs(r)) < 1e-9);
    printf("  ✓ test_rvol\n");
}
