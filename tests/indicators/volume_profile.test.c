#include <cxta/cxta.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_volume_profile(void) {
    const cxta_series_bar bars[] = {
        {0, 10.0, 11.0, 10.0, 10.5, 100.0},
        {1, 11.0, 12.0, 11.0, 11.5, 200.0},
        {2, 14.0, 16.0, 14.0, 13.5, 300.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    const cxta_vp_output out = cxta_volume_profile(&view, 3, 6);

    assert(fabs(out.poc - 14.5) < 1e-12);
    assert(fabs(out.val - 11.0) < 1e-12);
    assert(fabs(out.vah - 15.0) < 1e-12);
    assert(fabs(out.range_high - 16.0) < 1e-12);
    assert(fabs(out.range_low - 10.0) < 1e-12);
    printf("  ✓ test_volume_profile\n");
}
