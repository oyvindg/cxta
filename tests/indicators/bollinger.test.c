#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_bollinger(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 9.0, 0.0},
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 11.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    const cxta_bollinger_output out = cxta_bollinger(&view, 3, 2.0);
    assert(isfinite(out.middle));
    assert(isfinite(out.upper));
    assert(isfinite(out.lower));
    printf("  ✓ test_bollinger\n");
}
