#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_kst(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
    };
    const cxta_series_bar trend_bars[] = {
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 11.0, 0.0},
        {0, 0.0, 0.0, 0.0, 12.0, 0.0},
        {0, 0.0, 0.0, 0.0, 13.0, 0.0},
        {0, 0.0, 0.0, 0.0, 14.0, 0.0},
        {0, 0.0, 0.0, 0.0, 15.0, 0.0},
        {0, 0.0, 0.0, 0.0, 16.0, 0.0},
        {0, 0.0, 0.0, 0.0, 17.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 4, 3);
    const cxta_series_bar_view trend_view = cxta_series_bar_view_make(trend_bars, 8, 7);
    const cxta_kst_output flat = cxta_kst(&view, 10, 15, 20, 30, 9);
    const cxta_kst_output trend = cxta_kst(&trend_view, 2, 3, 4, 5, 3);
    assert(fabs(flat.line) < 1e-12);
    assert(fabs(flat.signal) < 1e-12);
    assert(fabs(flat.histogram) < 1e-12);
    assert(trend.line > 0.0);
    assert(trend.signal > 0.0);
    assert(fabs(trend.histogram - (trend.line - trend.signal)) < 1e-12);
    printf("  ✓ test_kst\n");
}
