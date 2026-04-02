/**
 * @file pivot.test.c
 * @brief Unit tests for cxseries pivot and structure helpers.
 *
 * Coverage:
 * - cxta_struct_pivot_args_parse
 * - cxta_struct_pivot_state_compute
 * - cxta_struct_structure_state_compute
 */

#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

static void test_pivot_args_parse(void) {
    cxta_struct_pivot_args out = {0, 0, 0};
    assert(cxta_struct_pivot_args_parse(NULL, 0, 9, &out) == 1);
    assert(out.left == 1);
    assert(out.right == 1);
    assert(out.lookback == 10);

    {
        const double args[3] = {3.0, 4.0, 50.0};
        assert(cxta_struct_pivot_args_parse(args, 3, 9, &out) == 1);
        assert(out.left == 3);
        assert(out.right == 4);
        assert(out.lookback == 50);
    }
    assert(cxta_struct_pivot_args_parse(NULL, 0, 0, NULL) == 0);
    printf("  ✓ test_pivot_args_parse\n");
}

static void test_pivot_state_compute(void) {
    const cxta_series_bar bars[7] = {
        {0, 1.2, 1.0, 2.0, 1.1, 100.0},
        {0, 1.7, 3.0, 1.5, 1.8, 100.0},
        {0, 1.2, 2.0, 1.0, 1.3, 100.0},
        {0, 1.8, 4.0, 1.6, 1.9, 100.0},
        {0, 1.4, 3.0, 1.2, 1.5, 100.0},
        {0, 2.1, 5.0, 1.8, 2.2, 100.0},
        {0, 1.6, 4.0, 1.4, 1.7, 100.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 7, 6);
    const cxta_struct_pivot_args args = {1, 1, 7};
    cxta_struct_pivot_state out = {0};
    assert(cxta_struct_pivot_state_compute(&view, &args, &out) == 1);
    assert(fabs(out.high - 5.0) < 1e-12);
    assert(fabs(out.is_high - 1.0) < 1e-12);
    assert(out.has_last_high == 1);
    assert(out.has_prev_high == 1);
    assert(fabs(out.prev_high - 4.0) < 1e-12);
    printf("  ✓ test_pivot_state_compute\n");
}

static void test_structure_state_compute(void) {
    cxta_struct_pivot_state pivot = {0};
    pivot.high = 5.0;
    pivot.prev_high = 4.0;
    pivot.low = 2.0;
    pivot.prev_low = 1.5;
    pivot.is_high = 1.0;
    pivot.is_low = 0.0;
    pivot.has_prev_high = 1;
    pivot.has_last_high = 1;
    pivot.has_prev_low = 1;
    pivot.has_last_low = 1;

    cxta_struct_structure_state out = {0};
    assert(cxta_struct_structure_state_compute(&pivot, &out) == 1);
    assert(fabs(out.hh - 1.0) < 1e-12);
    assert(fabs(out.hl - 0.0) < 1e-12);
    assert(fabs(out.last_high - 5.0) < 1e-12);
    assert(fabs(out.last_low - 2.0) < 1e-12);
    assert(out.strength >= 0.0 && out.strength <= 1.0);
    printf("  ✓ test_structure_state_compute\n");
}

void cxta_test_pivot(void) {
    test_pivot_args_parse();
    test_pivot_state_compute();
    test_structure_state_compute();
}
