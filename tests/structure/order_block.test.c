/**
 * @file order_block.test.c
 * @brief Unit tests for cxta_struct_order_block_state_compute.
 */

#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

static void test_order_block_null_args(void) {
    cxta_series_bar bars[1] = {{0, 1.0, 1.0, 1.0, 1.0, 100.0}};
    cxta_series_bar_view view = cxta_series_bar_view_make(bars, 1, 0);
    cxta_struct_pivot_args args = {1, 1, 10};
    cxta_struct_order_block_state out = {0};

    assert(cxta_struct_order_block_state_compute(NULL, &args, &out) == 0);
    assert(cxta_struct_order_block_state_compute(&view, NULL, &out) == 0);
    assert(cxta_struct_order_block_state_compute(&view, &args, NULL) == 0);
    printf("  ✓ test_order_block_null_args\n");
}

static void test_order_block_bullish_from_bos_up(void) {
    cxta_series_bar bars[] = {
        {0,  9.0, 10.0,  8.0,  8.5, 100.0},
        {1, 11.0, 12.0,  9.0, 11.5, 100.0},
        {2, 10.0, 11.0,  8.5,  9.0, 100.0},
        {3, 11.0, 13.0, 10.0, 12.0, 100.0},
        {4, 11.0, 12.0,  9.5, 10.0, 100.0},
        {5, 13.0, 14.0, 10.0, 13.5, 100.0},
    };

    cxta_series_bar_view view = cxta_series_bar_view_make(bars, 6, 5);
    cxta_struct_pivot_args args = {1, 1, 20};
    cxta_struct_order_block_state out = {0};
    assert(cxta_struct_order_block_state_compute(&view, &args, &out) == 1);
    assert(fabs(out.bullish - 1.0) < 1e-12);
    assert(fabs(out.bearish - 0.0) < 1e-12);
    assert(fabs(out.top - 11.0) < 1e-12);
    assert(fabs(out.bottom - 10.0) < 1e-12);
    assert(fabs(out.strength - 0.5) < 1e-12);
    printf("  ✓ test_order_block_bullish_from_bos_up\n");
}

static void test_order_block_bearish_from_bos_down(void) {
    cxta_series_bar bars[] = {
        {0, 8.0,  9.0, 7.0, 8.5, 100.0},
        {1, 9.0, 11.0, 8.0, 10.5, 100.0},
        {2, 10.0, 10.0, 7.0,  7.5, 100.0},
        {3, 8.0,  9.0, 6.0,  8.5, 100.0},
        {4, 8.5,  8.8, 6.5,  8.6, 100.0},
        {5, 7.8,  8.0, 5.5,  5.8, 100.0},
    };

    cxta_series_bar_view view = cxta_series_bar_view_make(bars, 6, 5);
    cxta_struct_pivot_args args = {1, 1, 20};
    cxta_struct_order_block_state out = {0};
    assert(cxta_struct_order_block_state_compute(&view, &args, &out) == 1);
    assert(fabs(out.bullish - 0.0) < 1e-12);
    assert(fabs(out.bearish - 1.0) < 1e-12);
    assert(fabs(out.top - 8.6) < 1e-12);
    assert(fabs(out.bottom - 8.5) < 1e-12);
    assert(fabs(out.strength - 1.0) < 1e-12);
    printf("  ✓ test_order_block_bearish_from_bos_down\n");
}

void cxta_test_order_block(void) {
    test_order_block_null_args();
    test_order_block_bullish_from_bos_up();
    test_order_block_bearish_from_bos_down();
}
