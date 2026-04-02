#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_wma(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 1.0, 0.0},
        {0, 0.0, 0.0, 0.0, 2.0, 0.0},
        {0, 0.0, 0.0, 0.0, 3.0, 0.0},
    };
    /* WMA(3): weights 1,2,3 → (1*1 + 2*2 + 3*3)/(1+2+3) = 14/6 */
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    assert(fabs(cxta_wma(&view, 3) - 14.0 / 6.0) < 1e-12);
    /* WMA with period > available bars falls back to all bars */
    assert(fabs(cxta_wma(&view, 10) - 14.0 / 6.0) < 1e-12);
    /* Single bar */
    const cxta_series_bar_view v1 = cxta_series_bar_view_make(bars, 3, 0);
    assert(fabs(cxta_wma(&v1, 3) - 1.0) < 1e-12);
    printf("  ✓ test_wma\n");
}
