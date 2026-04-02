/**
 * @file window.test.c
 * @brief Unit tests for cxseries window helpers.
 *
 * Coverage:
 * - cxta_series_window_highest_high
 * - cxta_series_window_lowest_low
 * - cxta_series_midpoint
 */

#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

static void test_window_extremes(void) {
    const cxta_series_bar bars[5] = {
        {0, 10.0, 11.0, 9.0, 10.5, 100.0},
        {0, 10.5, 12.0, 10.0, 11.0, 100.0},
        {0, 11.0, 13.5, 10.8, 12.5, 100.0},
        {0, 12.5, 12.8, 10.4, 11.2, 100.0},
        {0, 11.2, 11.6, 9.8, 10.1, 100.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 5, 4);

    double hi = 0.0;
    double lo = 0.0;
    assert(cxta_series_window_highest_high(&view, 1, 4, &hi) == 1);
    assert(cxta_series_window_lowest_low(&view, 1, 4, &lo) == 1);
    assert(fabs(hi - 13.5) < 1e-12);
    assert(fabs(lo - 9.8) < 1e-12);
    assert(fabs(cxta_series_midpoint(hi, lo) - 11.65) < 1e-12);
    printf("  ✓ test_window_extremes\n");
}

static void test_window_invalid_inputs(void) {
    const cxta_series_bar bars[2] = {
        {0, 1.0, 2.0, 0.5, 1.5, 10.0},
        {0, 1.5, 2.5, 1.0, 2.0, 10.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 2, 1);
    double out = 0.0;
    assert(cxta_series_window_highest_high(NULL, 0, 1, &out) == 0);
    assert(cxta_series_window_highest_high(&view, 1, 0, &out) == 0);
    assert(cxta_series_window_highest_high(&view, 0, 9, &out) == 0);
    assert(cxta_series_window_lowest_low(NULL, 0, 1, &out) == 0);
    assert(cxta_series_window_lowest_low(&view, 1, 0, &out) == 0);
    assert(cxta_series_window_lowest_low(&view, 0, 9, &out) == 0);
    printf("  ✓ test_window_invalid_inputs\n");
}

void cxta_test_window(void) {
    test_window_extremes();
    test_window_invalid_inputs();
}
