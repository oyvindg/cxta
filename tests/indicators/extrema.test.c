#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_extrema(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 10.0, 8.0, 0.0, 0.0},
        {0, 0.0, 12.0, 7.0, 0.0, 0.0},
        {0, 0.0, 11.0, 9.0, 0.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    const cxta_extrema_output ex = cxta_extrema(&view, 3);
    assert(fabs(ex.high - 12.0) < 1e-12);
    assert(fabs(ex.low  -  7.0) < 1e-12);
    assert(fabs(ex.mid  -  9.5) < 1e-12);
    /* period=1: only current bar */
    const cxta_extrema_output ex1 = cxta_extrema(&view, 1);
    assert(fabs(ex1.high - 11.0) < 1e-12);
    assert(fabs(ex1.low  -  9.0) < 1e-12);
    printf("  ✓ test_extrema\n");
}
