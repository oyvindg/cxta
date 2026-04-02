#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_hma(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
    };
    /* Constant series → HMA == constant */
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 4, 3);
    assert(fabs(cxta_hma(&view, 4) - 10.0) < 1e-9);
    /* Single bar */
    const cxta_series_bar_view v0 = cxta_series_bar_view_make(bars, 4, 0);
    assert(fabs(cxta_hma(&v0, 4) - 10.0) < 1e-9);
    printf("  ✓ test_hma\n");
}
