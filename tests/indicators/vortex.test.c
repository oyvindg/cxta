#include <cxta/cxta.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_vortex(void) {
    const cxta_series_bar bars[] = {
        {0, 10.0, 12.0, 8.0, 10.0, 0.0},
        {0, 10.0, 12.0, 8.0, 10.0, 0.0},
        {0, 10.0, 12.0, 8.0, 10.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    const cxta_vortex_output out = cxta_vortex(&view, 2);
    assert(fabs(out.vi_plus - 1.0) < 1e-12);
    assert(fabs(out.vi_minus - 1.0) < 1e-12);
    assert(fabs(out.trend) < 1e-12);
    printf("  ✓ test_vortex\n");
}
