/**
 * @file sfp.test.c
 * @brief Unit tests for cxta_struct_sfp_state_compute.
 */

#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

static cxta_series_bar_view make_view(cxta_series_bar* bar,
                              double open,
                              double high,
                              double low,
                              double close) {
    bar->timestamp = 0;
    bar->open = open;
    bar->high = high;
    bar->low = low;
    bar->close = close;
    bar->volume = 100.0;
    return cxta_series_bar_view_make(bar, 1, 0);
}

static void test_sfp_null_args(void) {
    cxta_series_bar bar;
    cxta_series_bar_view view = make_view(&bar, 100.0, 101.0, 99.0, 100.0);
    cxta_struct_pivot_state piv = {0};
    cxta_struct_sfp_state out = {0};
    assert(cxta_struct_sfp_state_compute(NULL, &piv, &out) == 0);
    assert(cxta_struct_sfp_state_compute(&view, NULL, &out) == 0);
    assert(cxta_struct_sfp_state_compute(&view, &piv, NULL) == 0);
    printf("  ✓ test_sfp_null_args\n");
}

static void test_sfp_high_detected(void) {
    cxta_series_bar bar;
    cxta_series_bar_view view = make_view(&bar, 100.2, 101.2, 98.9, 99.5);
    cxta_struct_pivot_state piv = {0};
    piv.high = 100.0;
    piv.has_last_high = 1;

    cxta_struct_sfp_state out = {0};
    assert(cxta_struct_sfp_state_compute(&view, &piv, &out) == 1);
    assert(fabs(out.sfp_high - 1.0) < 1e-12);
    assert(fabs(out.sfp_low - 0.0) < 1e-12);
    assert(fabs(out.level - 100.0) < 1e-12);
    printf("  ✓ test_sfp_high_detected\n");
}

static void test_sfp_low_detected(void) {
    cxta_series_bar bar;
    cxta_series_bar_view view = make_view(&bar, 90.2, 92.0, 88.8, 91.1);
    cxta_struct_pivot_state piv = {0};
    piv.low = 90.0;
    piv.has_last_low = 1;

    cxta_struct_sfp_state out = {0};
    assert(cxta_struct_sfp_state_compute(&view, &piv, &out) == 1);
    assert(fabs(out.sfp_high - 0.0) < 1e-12);
    assert(fabs(out.sfp_low - 1.0) < 1e-12);
    assert(fabs(out.level - 90.0) < 1e-12);
    printf("  ✓ test_sfp_low_detected\n");
}

void cxta_test_sfp(void) {
    test_sfp_null_args();
    test_sfp_high_detected();
    test_sfp_low_detected();
}
