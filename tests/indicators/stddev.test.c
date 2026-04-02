#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_stddev(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 2.0, 0.0},
        {0, 0.0, 0.0, 0.0, 4.0, 0.0},
        {0, 0.0, 0.0, 0.0, 4.0, 0.0},
        {0, 0.0, 0.0, 0.0, 4.0, 0.0},
        {0, 0.0, 0.0, 0.0, 5.0, 0.0},
        {0, 0.0, 0.0, 0.0, 5.0, 0.0},
        {0, 0.0, 0.0, 0.0, 7.0, 0.0},
        {0, 0.0, 0.0, 0.0, 9.0, 0.0},
    };
    /* population stddev of {2,4,4,4,5,5,7,9} = 2.0 */
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 8, 7);
    assert(fabs(cxta_stddev(&view, 8) - 2.0) < 1e-9);
    /* single bar → 0 */
    const cxta_series_bar_view v1 = cxta_series_bar_view_make(bars, 8, 0);
    assert(fabs(cxta_stddev(&v1, 8)) < 1e-12);
    printf("  ✓ test_stddev\n");
}
