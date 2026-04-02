/**
 * @file fvg.test.c
 * @brief Unit tests for cxta_struct_fvg_state_compute.
 */

#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

static void test_fvg_null_args(void) {
    cxta_struct_fvg_state out = {0};
    cxta_series_bar bars[1] = {{0, 1.0, 1.0, 1.0, 1.0, 100.0}};
    cxta_series_bar_view view = cxta_series_bar_view_make(bars, 1, 0);
    assert(cxta_struct_fvg_state_compute(NULL, 10, &out) == 0);
    assert(cxta_struct_fvg_state_compute(&view, 10, NULL) == 0);
    printf("  ✓ test_fvg_null_args\n");
}

static void test_fvg_bullish_and_fill(void) {
    cxta_series_bar bars[] = {
        {0,  9.5, 10.0,  9.0,  9.8, 100.0},
        {1, 10.8, 11.0, 10.5, 10.9, 100.0},
        {2, 12.2, 13.0, 12.0, 12.5, 100.0},
        {3, 10.5, 11.2,  9.5, 10.1, 100.0},
    };

    cxta_struct_fvg_state out = {0};
    cxta_series_bar_view at_gap = cxta_series_bar_view_make(bars, 4, 2);
    assert(cxta_struct_fvg_state_compute(&at_gap, 20, &out) == 1);
    assert(fabs(out.bullish - 1.0) < 1e-12);
    assert(fabs(out.bearish - 0.0) < 1e-12);
    assert(fabs(out.top - 12.0) < 1e-12);
    assert(fabs(out.bottom - 10.0) < 1e-12);
    assert(fabs(out.filled - 0.0) < 1e-12);

    cxta_series_bar_view at_fill = cxta_series_bar_view_make(bars, 4, 3);
    assert(cxta_struct_fvg_state_compute(&at_fill, 20, &out) == 1);
    assert(fabs(out.bullish - 1.0) < 1e-12);
    assert(fabs(out.filled - 1.0) < 1e-12);
    printf("  ✓ test_fvg_bullish_and_fill\n");
}

static void test_fvg_bearish_detected(void) {
    cxta_series_bar bars[] = {
        {0, 10.5, 11.0, 10.0, 10.2, 100.0},
        {1,  9.8, 10.0,  9.4,  9.6, 100.0},
        {2,  7.8,  8.0,  7.0,  7.5, 100.0},
    };

    cxta_struct_fvg_state out = {0};
    cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    assert(cxta_struct_fvg_state_compute(&view, 20, &out) == 1);
    assert(fabs(out.bullish - 0.0) < 1e-12);
    assert(fabs(out.bearish - 1.0) < 1e-12);
    assert(fabs(out.top - 10.0) < 1e-12);
    assert(fabs(out.bottom - 8.0) < 1e-12);
    assert(fabs(out.filled - 0.0) < 1e-12);
    printf("  ✓ test_fvg_bearish_detected\n");
}

void cxta_test_fvg(void) {
    test_fvg_null_args();
    test_fvg_bullish_and_fill();
    test_fvg_bearish_detected();
}
