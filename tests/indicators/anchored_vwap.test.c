#include <cxta/cxta.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_anchored_vwap(void) {
    const cxta_series_bar bars[] = {
        {0, 10.0, 10.0, 10.0, 10.0, 100.0},
        {3600, 12.0, 12.0, 12.0, 12.0, 100.0},
        {86400, 20.0, 20.0, 20.0, 20.0, 100.0},
        {90000, 22.0, 22.0, 22.0, 22.0, 100.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 4, 3);

    assert(fabs(cxta_anchored_vwap(&view, 0) - 21.0) < 1e-12);
    assert(fabs(cxta_anchored_vwap(&view, 3) - 18.0) < 1e-12);
    printf("  ✓ test_anchored_vwap\n");
}
