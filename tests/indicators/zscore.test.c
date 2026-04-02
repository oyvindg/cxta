#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_zscore(void) {
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
    /* mean=5, stddev=2, close=9 → z=(9-5)/2=2 */
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 8, 7);
    assert(fabs(cxta_zscore(&view, 8) - 2.0) < 1e-9);
    /* Flat series → z=0 */
    const cxta_series_bar flat[] = {
        {0, 0.0, 0.0, 0.0, 5.0, 0.0},
        {0, 0.0, 0.0, 0.0, 5.0, 0.0},
    };
    const cxta_series_bar_view vf = cxta_series_bar_view_make(flat, 2, 1);
    assert(fabs(cxta_zscore(&vf, 2)) < 1e-12);
    printf("  ✓ test_zscore\n");
}
