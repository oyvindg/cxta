#include <cxta/cxta.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_trendline(void) {
    const cxta_series_bar bars[] = {
        {0, 9.0, 10.0, 8.0, 9.0, 100.0},
        {1, 14.0, 15.0, 9.0, 14.0, 100.0},
        {2, 8.0, 12.0, 7.0, 8.0, 100.0},
        {3, 15.0, 16.0, 8.0, 15.0, 100.0},
        {4, 7.0, 13.0, 6.0, 7.0, 100.0},
        {5, 13.0, 14.0, 7.0, 13.0, 100.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 6, 5);
    const cxta_trendline_output out = cxta_trendline(&view, 1, 1);

    assert(fabs(out.upper - 17.0) < 1e-12);
    assert(fabs(out.lower - 5.5) < 1e-12);
    assert(fabs(out.upper_slope - 0.5) < 1e-12);
    assert(fabs(out.lower_slope + 0.5) < 1e-12);
    assert(fabs(out.upper_pivot - 16.0) < 1e-12);
    assert(fabs(out.upper_pivot_index - 3.0) < 1e-12);
    assert(fabs(out.lower_pivot - 6.0) < 1e-12);
    assert(fabs(out.lower_pivot_index - 4.0) < 1e-12);
    assert(fabs(out.active_index - 5.0) < 1e-12);
    printf("  ✓ test_trendline\n");
}
