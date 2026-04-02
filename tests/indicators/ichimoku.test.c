#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_ichimoku(void) {
    const cxta_series_bar bars[] = {
        {0, 7.0, 10.0, 6.0, 8.0, 100.0},
        {1, 8.0, 12.0, 7.0, 11.0, 100.0},
        {2, 11.0, 14.0, 8.0, 13.0, 100.0},
        {3, 13.0, 16.0, 10.0, 15.0, 100.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 4, 3);
    const cxta_ichimoku_output out = cxta_ichimoku(&view, 2, 3, 4);

    assert(fabs(out.tenkan - 12.0) < 1e-12);
    assert(fabs(out.kijun - 11.5) < 1e-12);
    assert(fabs(out.senkou_a - 11.75) < 1e-12);
    assert(fabs(out.senkou_b - 11.0) < 1e-12);
    assert(fabs(out.chikou - 15.0) < 1e-12);
    printf("  ✓ test_ichimoku\n");
}
