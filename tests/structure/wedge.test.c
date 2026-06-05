/**
 * @file wedge.test.c
 * @brief Unit tests for cxta_struct_wedge_state_compute.
 */

#include <cxta/cxta.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>

static const cxta_series_bar rising_wedge_bars[] = {
    {0, 103.0, 106.0, 101.0, 103.0, 1000.0},
    {1, 109.0, 110.0, 105.0, 109.0, 1000.0},
    {2, 101.0, 107.0, 100.0, 101.0, 1000.0},
    {3, 113.0, 114.0, 108.0, 113.0, 1000.0},
    {4, 106.0, 110.0, 101.0, 106.0, 1000.0},
    {5, 112.0, 113.0, 110.0, 112.0, 1000.0},
    {6, 111.0, 112.0, 111.0, 111.0, 1000.0},
    {7, 115.0, 116.0, 113.0, 115.0, 1000.0},
    {8, 114.0, 115.0, 112.0, 114.0, 1000.0},
    {9, 114.0, 114.0, 114.0, 114.0, 1000.0},
    {10, 110.0, 114.0, 109.0, 110.0, 1000.0},
};

static const cxta_series_bar flat_bars[] = {
    {0, 100.0, 101.0, 99.0, 100.0, 1000.0},
    {1, 100.0, 101.0, 99.0, 100.0, 1000.0},
    {2, 100.0, 101.0, 99.0, 100.0, 1000.0},
};

static const cxta_series_bar broadening_bars[] = {
    {0, 103.0, 106.0, 101.0, 103.0, 1000.0},
    {1, 109.0, 110.0, 105.0, 109.0, 1000.0},
    {2, 101.0, 107.0, 100.0, 101.0, 1000.0},
    {3, 114.0, 115.0, 108.0, 114.0, 1000.0},
    {4, 106.0, 110.0, 105.0, 106.0, 1000.0},
    {5, 112.0, 113.0, 110.0, 112.0, 1000.0},
    {6, 111.0, 112.0, 111.0, 111.0, 1000.0},
    {7, 120.0, 121.0, 113.0, 120.0, 1000.0},
    {8, 113.0, 115.0, 109.0, 113.0, 1000.0},
    {9, 113.0, 114.0, 113.0, 113.0, 1000.0},
    {10, 134.0, 135.0, 121.0, 134.0, 1000.0},
};

static void test_wedge_args_parse(void) {
    cxta_struct_wedge_args out = {0};

    assert(cxta_struct_wedge_args_parse(NULL, 0u, 10u, &out) == 1);
    assert(out.left == 2);
    assert(out.right == 2);
    assert(out.lookback == 40);
    assert(fabs(out.max_width_ratio - 0.85) < 1e-12);
    assert(out.auto_left_right == 0);

    {
        const double args[] = {0.0, 0.0, 10.0, 0.7};
        assert(cxta_struct_wedge_args_parse(args, 4u, 10u, &out) == 1);
        assert(out.left == 1);
        assert(out.right == 1);
        assert(out.lookback == 10);
        assert(fabs(out.max_width_ratio - 0.7) < 1e-12);
        assert(out.auto_left_right == 1);
    }

    assert(cxta_struct_wedge_args_parse(NULL, 0u, 0u, NULL) == 0);
    printf("  ✓ test_wedge_args_parse\n");
}

static void test_wedge_state_compute_explicit_strength(void) {
    const cxta_series_bar_view view =
        cxta_series_bar_view_make(rising_wedge_bars, 11u, 10u);
    const double args[] = {1.0, 1.0, 10.0};
    cxta_struct_wedge_args parsed = {0};
    cxta_struct_wedge_state out = {0};

    assert(cxta_struct_wedge_args_parse(args, 3u, view.index, &parsed) == 1);
    assert(cxta_struct_wedge_state_compute(&view, &parsed, &out) == 1);
    assert(out.has_pivots == 1);
    assert(out.active == 1.0);
    assert(out.rising == 1.0);
    assert(out.falling == 0.0);
    assert(out.breakdown == 1.0);
    assert(out.breakout == 0.0);
    assert(isfinite(out.upper));
    assert(isfinite(out.lower));
    assert(isfinite(out.width));
    printf("  ✓ test_wedge_state_compute_explicit_strength\n");
}

static void test_wedge_state_compute_auto_detect(void) {
    const cxta_series_bar_view view =
        cxta_series_bar_view_make(rising_wedge_bars, 11u, 10u);
    const double args[] = {0.0, 0.0, 10.0};
    cxta_struct_wedge_args parsed = {0};
    cxta_struct_wedge_state out = {0};

    assert(cxta_struct_wedge_args_parse(args, 3u, view.index, &parsed) == 1);
    assert(cxta_struct_wedge_state_compute(&view, &parsed, &out) == 1);
    assert(out.has_pivots == 1);
    assert(out.active == 1.0);
    assert(out.rising == 1.0);
    assert(out.breakdown == 1.0);
    printf("  ✓ test_wedge_state_compute_auto_detect\n");
}

static void test_wedge_mask_geometry(void) {
    const cxta_series_bar_view view = cxta_series_bar_view_make(flat_bars, 3u, 2u);
    cxta_struct_wedge_state out = {0};

    assert(cxta_struct_wedge_state_compute(&view, NULL, &out) == 1);
    assert(out.active == 0.0);
    cxta_struct_wedge_state_mask_geometry(&out);
    assert(isnan(out.upper));
    assert(isnan(out.lower));
    assert(isnan(out.width));
    assert(isnan(out.upper_slope));
    assert(isnan(out.lower_slope));
    printf("  ✓ test_wedge_mask_geometry\n");
}

static void test_broadening_state_compute_close_breakout(void) {
    const cxta_series_bar_view view =
        cxta_series_bar_view_make(broadening_bars, 11u, 10u);
    const double args[] = {1.0, 1.0, 10.0, 1.05, 0.0};
    cxta_struct_broadening_args parsed = {0};
    cxta_struct_broadening_state out = {0};

    assert(cxta_struct_broadening_args_parse(args, 5u, view.index, &parsed) == 1);
    assert(parsed.breakout_source == 0);
    assert(cxta_struct_broadening_state_compute(&view, &parsed, &out) == 1);
    assert(out.has_pivots == 1);
    assert(out.active == 1.0);
    assert(out.rising == 1.0);
    assert(out.breakout == 1.0);
    assert(out.breakdown == 0.0);
    assert(out.direction == 1.0);
    assert(out.strength > 0.0);
    assert(isfinite(out.resistance));
    assert(isfinite(out.support));
    printf("  ✓ test_broadening_state_compute_close_breakout\n");
}

static void test_broadening_state_compute_open_breakout_source(void) {
    const cxta_series_bar bars[] = {
        {0, 103.0, 106.0, 101.0, 103.0, 1000.0},
        {1, 109.0, 110.0, 105.0, 109.0, 1000.0},
        {2, 101.0, 107.0, 100.0, 101.0, 1000.0},
        {3, 114.0, 115.0, 108.0, 114.0, 1000.0},
        {4, 106.0, 110.0, 101.0, 106.0, 1000.0},
        {5, 112.0, 113.0, 110.0, 112.0, 1000.0},
        {6, 111.0, 112.0, 111.0, 111.0, 1000.0},
        {7, 120.0, 121.0, 113.0, 120.0, 1000.0},
        {8, 113.0, 115.0, 109.0, 113.0, 1000.0},
        {9, 113.0, 114.0, 113.0, 113.0, 1000.0},
        {10, 134.0, 135.0, 121.0, 124.0, 1000.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 11u, 10u);
    const double close_args[] = {1.0, 1.0, 10.0, 1.05, 0.0};
    const double open_args[] = {1.0, 1.0, 10.0, 1.05, 1.0};
    cxta_struct_broadening_args parsed = {0};
    cxta_struct_broadening_state out = {0};

    assert(cxta_struct_broadening_args_parse(close_args, 5u, view.index, &parsed) == 1);
    assert(cxta_struct_broadening_state_compute(&view, &parsed, &out) == 1);
    assert(out.breakout == 0.0);

    assert(cxta_struct_broadening_args_parse(open_args, 5u, view.index, &parsed) == 1);
    assert(parsed.breakout_source == 1);
    assert(cxta_struct_broadening_state_compute(&view, &parsed, &out) == 1);
    assert(out.breakout == 1.0);
    printf("  ✓ test_broadening_state_compute_open_breakout_source\n");
}

static void test_broadening_auto_detect_prefers_outer_pivots_over_recent_noise(void) {
    const cxta_series_bar bars[] = {
        {0, 500.0, 503.0, 500.0, 502.0, 1000.0},
        {1, 504.0, 505.0, 502.0, 504.0, 1000.0},
        {2, 500.0, 503.0, 499.0, 500.0, 1000.0},
        {3, 504.0, 506.0, 501.0, 504.0, 1000.0},
        {4, 506.0, 507.0, 501.0, 506.0, 1000.0},
        {5, 509.0, 510.0, 500.0, 509.0, 1000.0},
        {6, 494.0, 504.0, 492.0, 494.0, 1000.0},
        {7, 502.0, 504.0, 500.0, 502.0, 1000.0},
        {8, 503.0, 506.0, 498.0, 503.0, 1000.0},
        {9, 501.0, 503.0, 501.0, 501.0, 1000.0},
        {10, 510.0, 515.0, 487.0, 510.0, 1000.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 11u, 10u);
    const double args[] = {0.0, 0.0, 11.0, 1.05, 0.0};
    cxta_struct_broadening_args parsed = {0};
    cxta_struct_broadening_state out = {0};

    assert(cxta_struct_broadening_args_parse(args, 5u, view.index, &parsed) == 1);
    assert(parsed.auto_left_right == 1);
    assert(cxta_struct_broadening_state_compute(&view, &parsed, &out) == 1);
    assert(out.has_pivots == 1);
    assert(out.active == 1.0);
    assert(out.resistance_pivot_index == 5.0);
    assert(out.support_pivot_index == 6.0);
    assert(out.resistance > 514.0);
    assert(out.support < 488.0);
    printf("  ✓ test_broadening_auto_detect_prefers_outer_pivots_over_recent_noise\n");
}

void cxta_test_structure_wedge(void) {
    test_wedge_args_parse();
    test_wedge_state_compute_explicit_strength();
    test_wedge_state_compute_auto_detect();
    test_wedge_mask_geometry();
    test_broadening_state_compute_close_breakout();
    test_broadening_state_compute_open_breakout_source();
    test_broadening_auto_detect_prefers_outer_pivots_over_recent_noise();
}
