/**
 * @file liquidity.test.c
 * @brief Unit tests for cxta_struct_liquidity_state_compute.
 */

#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

static cxta_series_bar_view make_one_bar_view(cxta_series_bar* bar,
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

static void test_liquidity_null_args(void) {
    cxta_series_bar bar;
    cxta_series_bar_view view = make_one_bar_view(&bar, 100.0, 101.0, 99.0, 100.0);
    cxta_struct_pivot_state piv = {0};
    cxta_struct_liquidity_state out = {0};

    assert(cxta_struct_liquidity_state_compute(NULL, &piv, 0.001, &out) == 0);
    assert(cxta_struct_liquidity_state_compute(&view, NULL, 0.001, &out) == 0);
    assert(cxta_struct_liquidity_state_compute(&view, &piv, 0.001, NULL) == 0);
    printf("  ✓ test_liquidity_null_args\n");
}

static void test_liquidity_buy_side_swept(void) {
    cxta_series_bar bar;
    cxta_series_bar_view view = make_one_bar_view(&bar, 100.5, 101.2, 98.8, 99.5);

    cxta_struct_pivot_state piv = {0};
    piv.high = 100.00;
    piv.prev_high = 100.04;
    piv.has_last_high = 1;
    piv.has_prev_high = 1;

    cxta_struct_liquidity_state out = {0};
    assert(cxta_struct_liquidity_state_compute(&view, &piv, 0.001, &out) == 1);
    assert(fabs(out.buy_side - 1.0) < 1e-12);
    assert(fabs(out.sell_side - 0.0) < 1e-12);
    assert(fabs(out.level - 100.02) < 1e-12);
    assert(fabs(out.swept - 1.0) < 1e-12);
    printf("  ✓ test_liquidity_buy_side_swept\n");
}

static void test_liquidity_sell_side_swept(void) {
    cxta_series_bar bar;
    cxta_series_bar_view view = make_one_bar_view(&bar, 90.5, 92.0, 89.0, 91.0);

    cxta_struct_pivot_state piv = {0};
    piv.low = 90.00;
    piv.prev_low = 89.97;
    piv.has_last_low = 1;
    piv.has_prev_low = 1;

    cxta_struct_liquidity_state out = {0};
    assert(cxta_struct_liquidity_state_compute(&view, &piv, 0.001, &out) == 1);
    assert(fabs(out.buy_side - 0.0) < 1e-12);
    assert(fabs(out.sell_side - 1.0) < 1e-12);
    assert(fabs(out.level - 89.985) < 1e-12);
    assert(fabs(out.swept - 1.0) < 1e-12);
    printf("  ✓ test_liquidity_sell_side_swept\n");
}

void cxta_test_liquidity(void) {
    test_liquidity_null_args();
    test_liquidity_buy_side_swept();
    test_liquidity_sell_side_swept();
}
