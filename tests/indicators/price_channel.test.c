#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_price_channel(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 10.0, 5.0, 9.0, 0.0},
        {0, 0.0, 11.0, 6.0, 10.0, 0.0},
        {0, 0.0, 12.0, 7.0, 11.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    const cxta_channel_output out = cxta_price_channel(&view, 2);
    assert(fabs(out.upper - 12.0) < 1e-12);
    assert(fabs(out.lower - 6.0) < 1e-12);
    printf("  ✓ test_price_channel\n");
}
