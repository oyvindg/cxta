#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_mfi(void) {
    const cxta_series_bar bars[] = {
        {0, 10.0, 12.0, 8.0, 10.0, 100.0},
        {0, 11.0, 13.0, 9.0, 11.0, 100.0},
        {0, 12.0, 14.0, 10.0, 12.0, 100.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    assert(fabs(cxta_mfi(&view, 2) - 100.0) < 1e-12);
    printf("  ✓ test_mfi\n");
}
