/**
 * @file view.test.c
 * @brief Unit tests for cxseries AoS/SoA view helpers.
 *
 * Coverage:
 * - cxta_series_bar_view_* helpers
 * - cxta_series_soa_view_* helpers
 * - cxta_series_scalar_view_* helpers
 */

#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

static void test_bar_view(void) {
    const cxta_series_bar bars[3] = {
        {0, 1.0, 2.0, 0.5, 1.5, 100.0},
        {0, 2.0, 3.0, 1.5, 2.5, 200.0},
        {0, 3.0, 4.0, 2.5, 3.5, 300.0}
    };

    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 99);
    assert(cxta_series_bar_view_valid(&view) == 1);
    assert(view.index == 2);

    const cxta_series_bar* current = cxta_series_bar_view_current(&view);
    assert(current);
    assert(fabs(current->close - 3.5) < 1e-12);

    const cxta_series_bar* first = cxta_series_bar_view_at(&view, 0);
    assert(first);
    assert(fabs(first->open - 1.0) < 1e-12);

    assert(cxta_series_bar_view_at(&view, 9) == NULL);

    assert(fabs(cxta_series_typical_price(&bars[1]) - ((3.0 + 1.5 + 2.5) / 3.0)) < 1e-12);
    assert(fabs(cxta_series_typical_price(NULL) - 0.0) < 1e-12);
    printf("  ✓ test_bar_view\n");
}

static void test_soa_view(void) {
    const double open[3] = {1.0, 2.0, 3.0};
    const double high[3] = {2.0, 3.0, 4.0};
    const double low[3] = {0.5, 1.5, 2.5};
    const double close[3] = {1.5, 2.5, 3.5};
    const double volume[3] = {100.0, 200.0, 300.0};

    const cxta_series_soa_view view = cxta_series_soa_view_make(open, high, low, close, volume, 3, 1);
    assert(cxta_series_soa_view_valid(&view) == 1);

    double out = 0.0;
    assert(cxta_series_soa_close_at(&view, 2, &out) == 1);
    assert(fabs(out - 3.5) < 1e-12);

    assert(cxta_series_soa_close_current(&view, &out) == 1);
    assert(fabs(out - 2.5) < 1e-12);

    assert(cxta_series_soa_close_at(&view, 9, &out) == 0);
    assert(cxta_series_soa_close_current(NULL, &out) == 0);
    printf("  ✓ test_soa_view\n");
}

static void test_scalar_view(void) {
    const double values[4] = {10.0, 20.0, 30.0, 40.0};

    const cxta_series_scalar_view view = cxta_series_scalar_view_make(values, 4, 99);
    assert(cxta_series_scalar_view_valid(&view) == 1);
    assert(view.index == 3);

    double out = 0.0;
    assert(cxta_series_scalar_at(&view, 1, &out) == 1);
    assert(fabs(out - 20.0) < 1e-12);

    assert(cxta_series_scalar_current(&view, &out) == 1);
    assert(fabs(out - 40.0) < 1e-12);

    assert(cxta_series_scalar_at(&view, 9, &out) == 0);
    assert(cxta_series_scalar_current(NULL, &out) == 0);
    assert(cxta_series_scalar_view_valid(NULL) == 0);

    const cxta_series_scalar_view invalid = cxta_series_scalar_view_make(NULL, 4, 0);
    assert(cxta_series_scalar_view_valid(&invalid) == 0);

    printf("  ✓ test_scalar_view\n");
}

static void test_session_start_index(void) {
    /* Day 1: timestamps 86400, 86700, 87000 (all same UTC day, day=1) */
    /* Day 2: timestamps 172800, 173100        (UTC day=2)             */
    const cxta_series_bar bars[5] = {
        {86400,  100.0, 110.0, 99.0,  105.0, 1000.0},
        {86700,  105.0, 112.0, 104.0, 108.0, 1000.0},
        {87000,  108.0, 113.0, 107.0, 111.0, 1000.0},
        {172800, 111.0, 115.0, 110.0, 114.0, 1000.0},
        {173100, 114.0, 116.0, 113.0, 115.0, 1000.0}
    };

    /* Day 1 bars all point back to index 0 */
    assert(cxta_series_session_start_index(bars, 5, 0) == 0);
    assert(cxta_series_session_start_index(bars, 5, 1) == 0);
    assert(cxta_series_session_start_index(bars, 5, 2) == 0);

    /* Day 2 bars start at index 3 */
    assert(cxta_series_session_start_index(bars, 5, 3) == 3);
    assert(cxta_series_session_start_index(bars, 5, 4) == 3);

    /* Edge cases */
    assert(cxta_series_session_start_index(NULL, 5, 0) == 0);
    assert(cxta_series_session_start_index(bars, 0, 0) == 0);
    /* idx clamped to size-1 */
    assert(cxta_series_session_start_index(bars, 5, 99) == 3);

    printf("  \xe2\x9c\x93 test_session_start_index\n");
}

static void test_session_high_low(void) {
    /* Day 1: 7 bars — OR period=5: high=113, low=106 */
    const uint64_t d1 = 1704153600ULL;
    const uint64_t d2 = 1704240000ULL;
    const uint64_t step = 300ULL;
    const cxta_series_bar bars[10] = {
        {d1+0*step, 100.0, 110.0, 108.0, 109.0, 1000.0},
        {d1+1*step, 109.0, 112.0, 109.0, 111.0, 1000.0},
        {d1+2*step, 111.0, 111.0, 107.0, 108.0, 1000.0},
        {d1+3*step, 108.0, 113.0, 110.0, 112.0, 1000.0},
        {d1+4*step, 112.0, 109.0, 106.0, 107.0, 1000.0}, /* last OR bar  */
        {d1+5*step, 107.0, 115.0, 107.0, 115.0, 1000.0}, /* first post-OR */
        {d1+6*step, 115.0, 116.0, 113.0, 114.0, 1000.0},
        {d2+0*step, 114.0, 118.0, 114.0, 117.0, 1000.0},
        {d2+1*step, 117.0, 119.0, 116.0, 118.0, 1000.0},
        {d2+2*step, 118.0, 120.0, 117.0, 119.0, 1000.0},
    };

    /* Within OR: NaN */
    assert(isnan(cxta_series_session_high(bars, 10, 4, 5)));
    assert(isnan(cxta_series_session_low (bars, 10, 4, 5)));

    /* First post-OR bar: correct values */
    assert(fabs(cxta_series_session_high(bars, 10, 5, 5) - 113.0) < 1e-9);
    assert(fabs(cxta_series_session_low (bars, 10, 5, 5) - 106.0) < 1e-9);

    /* Stable across subsequent day-1 bars */
    assert(fabs(cxta_series_session_high(bars, 10, 6, 5) - 113.0) < 1e-9);
    assert(fabs(cxta_series_session_low (bars, 10, 6, 5) - 106.0) < 1e-9);

    /* Day 2 resets: offset 0-2 < 5 -> NaN */
    assert(isnan(cxta_series_session_high(bars, 10, 7, 5)));
    assert(isnan(cxta_series_session_low (bars, 10, 9, 5)));

    /* period=1: fires on second bar */
    assert(isnan(cxta_series_session_high(bars, 10, 0, 1)));
    assert(fabs(cxta_series_session_high(bars, 10, 1, 1) - 110.0) < 1e-9);
    assert(fabs(cxta_series_session_low (bars, 10, 1, 1) - 108.0) < 1e-9);

    /* Edge: NULL / size=0 / period=0 */
    assert(isnan(cxta_series_session_high(NULL, 10, 5, 5)));
    assert(isnan(cxta_series_session_high(bars,  0, 5, 5)));
    assert(isnan(cxta_series_session_high(bars, 10, 5, 0)));

    printf("  \xe2\x9c\x93 test_session_high_low\n");
}

void cxta_test_bar(void) {
    test_bar_view();
    test_soa_view();
    test_scalar_view();
    test_session_start_index();
    test_session_high_low();
}
