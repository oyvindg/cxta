#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_keltner(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 10.0, 8.0, 9.0, 0.0},
        {0, 0.0, 11.0, 9.0, 10.0, 0.0},
        {0, 0.0, 12.0, 10.0, 11.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    const cxta_channel_output out = cxta_keltner(&view, 2, 2.0, 2);
    assert(fabs(out.middle - 10.555555555555555) < 1e-12);
    assert(fabs(out.width - 8.0) < 1e-12);
    printf("  ✓ test_keltner\n");
}
