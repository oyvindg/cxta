#include <cxta/cxta.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_ttm_squeeze(void) {
    const cxta_series_bar bars[] = {
        {0, 10.0, 12.0, 8.0, 10.0, 100.0},
        {1, 10.0, 12.0, 8.0, 10.0, 100.0},
        {2, 10.0, 12.0, 8.0, 10.0, 100.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    const cxta_squeeze_output out = cxta_ttm_squeeze(&view, 3, 2.0, 3, 1.5, 3);

    assert(fabs(out.squeeze_on - 1.0) < 1e-12);
    assert(fabs(out.squeeze_off - 0.0) < 1e-12);
    assert(fabs(out.momentum - 0.0) < 1e-12);
    printf("  ✓ test_ttm_squeeze\n");
}
