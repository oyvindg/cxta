#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_volume_sma(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 1.0, 10.0},
        {0, 0.0, 0.0, 0.0, 2.0, 20.0},
        {0, 0.0, 0.0, 0.0, 3.0, 30.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    assert(fabs(cxta_volume_sma(&view, 2) - 25.0) < 1e-12);
    printf("  ✓ test_volume_sma\n");
}
