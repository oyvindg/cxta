/**
 * @file swing_anchor_vwap.test.c
 * @brief Unit tests for rolling swing-anchor adaptive VWAP helper.
 */

#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

static void test_swing_anchor_vwap_args_parse_defaults(void) {
    cxta_struct_vwap_args args = {0};
    assert(cxta_struct_vwap_args_parse(NULL, 0, &args) == 1);
    assert(args.swing_period == 50);
    assert(fabs(args.apt - 20.0) < 1e-12);
    assert(args.use_adapt == 0);
    assert(fabs(args.vol_bias - 10.0) < 1e-12);
    assert(args.atr_period == 50);
    assert(fabs(args.min_apt - 5.0) < 1e-12);
    assert(fabs(args.max_apt - 300.0) < 1e-12);
    printf("  ✓ test_swing_anchor_vwap_args_parse_defaults\n");
}

static void test_swing_anchor_vwap_detects_reanchor_and_structure_event(void) {
    const cxta_series_bar bars[4] = {
        {0, 9.4, 10.0, 9.0, 9.6, 100.0},
        {1, 10.0, 11.0, 9.5, 10.5, 100.0},
        {2, 9.1, 10.5, 8.5, 9.0, 100.0},
        {3, 10.8, 12.0, 9.0, 11.5, 100.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 4, 3);
    const double raw_args[4] = {2.0, 20.0, 0.0, 10.0};
    cxta_struct_vwap_args args = {0};
    cxta_struct_vwap_result out = {0};

    assert(cxta_struct_vwap_args_parse(raw_args, 4, &args) == 1);
    assert(cxta_struct_vwap_compute(&view, &args, &out) == 1);

    assert(fabs(out.direction - 1.0) < 1e-12);
    assert(fabs(out.reanchor - 1.0) < 1e-12);
    assert(fabs(out.ll - 1.0) < 1e-12);
    assert(fabs(out.anchor_price - 8.5) < 1e-12);
    assert(fabs(out.anchor_bars - 1.0) < 1e-12);
    assert(fabs(out.last_high - 12.0) < 1e-12);
    assert(fabs(out.last_low - 8.5) < 1e-12);
    assert(isfinite(out.value));
    assert(out.value > 0.0);
    printf("  ✓ test_swing_anchor_vwap_detects_reanchor_and_structure_event\n");
}

void cxta_test_vwap(void) {
    test_swing_anchor_vwap_args_parse_defaults();
    test_swing_anchor_vwap_detects_reanchor_and_structure_event();
}
