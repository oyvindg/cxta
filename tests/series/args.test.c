/**
 * @file args.test.c
 * @brief Unit tests for bar series argument tuple parsers.
 *
 * Coverage:
 * - cxta_series_bar_args_parse
 * - cxta_series_oc_pair_args_parse
 */

#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

static void test_bar_args_parse(void) {
    const double args[4] = {100.0, 105.0, 99.0, 103.0};
    cxta_series_bar_args out = {0};
    assert(cxta_series_bar_args_parse(args, 4, &out) == 1);
    assert(fabs(out.open - 100.0) < 1e-12);
    assert(fabs(out.high - 105.0) < 1e-12);
    assert(fabs(out.low - 99.0) < 1e-12);
    assert(fabs(out.close - 103.0) < 1e-12);
    assert(cxta_series_bar_args_parse(args, 3, &out) == 0);
    assert(cxta_series_bar_args_parse(NULL, 4, &out) == 0);
    assert(cxta_series_bar_args_parse(args, 4, NULL) == 0);
    printf("  ✓ test_bar_args_parse\n");
}

static void test_oc_pair_args_parse(void) {
    const double args[4] = {101.0, 106.0, 105.0, 100.0};
    cxta_series_oc_pair_args out = {0};
    assert(cxta_series_oc_pair_args_parse(args, 4, &out) == 1);
    assert(fabs(out.open - 101.0) < 1e-12);
    assert(fabs(out.close - 106.0) < 1e-12);
    assert(fabs(out.prev_open - 105.0) < 1e-12);
    assert(fabs(out.prev_close - 100.0) < 1e-12);
    assert(cxta_series_oc_pair_args_parse(args, 3, &out) == 0);
    assert(cxta_series_oc_pair_args_parse(NULL, 4, &out) == 0);
    assert(cxta_series_oc_pair_args_parse(args, 4, NULL) == 0);
    printf("  ✓ test_oc_pair_args_parse\n");
}

void cxta_test_args(void) {
    test_bar_args_parse();
    test_oc_pair_args_parse();
}
