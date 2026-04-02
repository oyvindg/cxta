#include <cxta/cxta.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_stochrsi(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 4, 3);
    const cxta_stochrsi_output out = cxta_stochrsi(&view, 14, 14, 3, 3);
    assert(fabs(out.k - 50.0) < 1e-12);
    assert(fabs(out.d - 50.0) < 1e-12);
    printf("  ✓ test_stochrsi\n");
}
