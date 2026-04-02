/**
 * @file bos.test.c
 * @brief Unit tests for cxta_struct_bos_state_compute.
 *
 * Tests covered:
 * - NULL argument handling
 * - No BOS when close is inside the pivot range
 * - BOS up (trend continuation — bullish structure, close breaks high)
 * - BOS down / CHoCH down (bullish structure, close breaks low)
 * - CHoCH up (bearish structure, close breaks above last pivot high)
 * - Incomplete pivot history produces no CHoCH
 */

#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

/* Construct a minimal single-bar view with the given close. */
static cxta_series_bar_view make_close_view(cxta_series_bar* buf, double close_val) {
    buf->timestamp = 0;
    buf->open      = close_val;
    buf->high      = close_val + 0.5;
    buf->low       = close_val - 0.5;
    buf->close     = close_val;
    buf->volume    = 100.0;
    return cxta_series_bar_view_make(buf, 1, 0);
}

/* Pivot state representing a bullish structure: HH (5.0 > 4.0) + HL (1.2 > 1.0). */
static cxta_struct_pivot_state make_bullish_pivot(void) {
    cxta_struct_pivot_state p = {0};
    p.high           = 5.0;
    p.prev_high      = 4.0;
    p.low            = 1.2;
    p.prev_low       = 1.0;
    p.has_last_high  = 1;
    p.has_prev_high  = 1;
    p.has_last_low   = 1;
    p.has_prev_low   = 1;
    return p;
}

/* Pivot state representing a bearish structure: LH (4.5 < 6.0) + LL (0.8 < 1.5). */
static cxta_struct_pivot_state make_bearish_pivot(void) {
    cxta_struct_pivot_state p = {0};
    p.high           = 4.5;
    p.prev_high      = 6.0;
    p.low            = 0.8;
    p.prev_low       = 1.5;
    p.has_last_high  = 1;
    p.has_prev_high  = 1;
    p.has_last_low   = 1;
    p.has_prev_low   = 1;
    return p;
}

static void test_bos_null_args(void) {
    cxta_series_bar         buf;
    cxta_struct_pivot_state piv = make_bullish_pivot();
    cxta_struct_bos_state   out = {0};
    cxta_series_bar_view    view = make_close_view(&buf, 3.0);

    assert(cxta_struct_bos_state_compute(NULL,  &piv, &out) == 0);
    assert(cxta_struct_bos_state_compute(&view, NULL, &out) == 0);
    assert(cxta_struct_bos_state_compute(&view, &piv, NULL) == 0);
    printf("  ✓ test_bos_null_args\n");
}

static void test_bos_no_break(void) {
    cxta_series_bar         buf;
    cxta_struct_pivot_state piv  = make_bullish_pivot(); /* last_high=5.0, last_low=1.2 */
    cxta_struct_bos_state   out  = {0};
    /* close=3.0: 1.2 < 3.0 < 5.0 — no break */
    cxta_series_bar_view    view = make_close_view(&buf, 3.0);

    assert(cxta_struct_bos_state_compute(&view, &piv, &out) == 1);
    assert(fabs(out.bos_up    - 0.0) < 1e-12);
    assert(fabs(out.bos_down  - 0.0) < 1e-12);
    assert(fabs(out.level     - 0.0) < 1e-12);
    assert(fabs(out.choch_up  - 0.0) < 1e-12);
    assert(fabs(out.choch_down - 0.0) < 1e-12);
    printf("  ✓ test_bos_no_break\n");
}

static void test_bos_up_continuation(void) {
    cxta_series_bar         buf;
    /* Bullish structure: HH + HL.  Close breaks above last_high=5.0. */
    /* This is a trend-continuation BOS, not a CHoCH.                  */
    cxta_struct_pivot_state piv  = make_bullish_pivot();
    cxta_struct_bos_state   out  = {0};
    cxta_series_bar_view    view = make_close_view(&buf, 5.5);

    assert(cxta_struct_bos_state_compute(&view, &piv, &out) == 1);
    assert(fabs(out.bos_up    - 1.0) < 1e-12);
    assert(fabs(out.level     - 5.0) < 1e-12);
    assert(fabs(out.choch_up  - 0.0) < 1e-12); /* not a reversal */
    assert(fabs(out.bos_down  - 0.0) < 1e-12);
    assert(fabs(out.choch_down - 0.0) < 1e-12);
    printf("  ✓ test_bos_up_continuation\n");
}

static void test_bos_down_choch(void) {
    cxta_series_bar         buf;
    /* Bullish structure: HH + HL.  Close breaks below last_low=1.2. */
    /* Breaking downward inside a bullish structure = CHoCH down.     */
    cxta_struct_pivot_state piv  = make_bullish_pivot();
    cxta_struct_bos_state   out  = {0};
    cxta_series_bar_view    view = make_close_view(&buf, 1.1);

    assert(cxta_struct_bos_state_compute(&view, &piv, &out) == 1);
    assert(fabs(out.bos_down   - 1.0) < 1e-12);
    assert(fabs(out.level      - 1.2) < 1e-12);
    assert(fabs(out.choch_down - 1.0) < 1e-12);
    assert(fabs(out.bos_up     - 0.0) < 1e-12);
    assert(fabs(out.choch_up   - 0.0) < 1e-12);
    printf("  ✓ test_bos_down_choch\n");
}

static void test_bos_up_choch(void) {
    cxta_series_bar         buf;
    /* Bearish structure: LH + LL.  Close breaks above last_high=4.5. */
    /* Breaking upward inside a bearish structure = CHoCH up.          */
    cxta_struct_pivot_state piv  = make_bearish_pivot();
    cxta_struct_bos_state   out  = {0};
    cxta_series_bar_view    view = make_close_view(&buf, 5.0);

    assert(cxta_struct_bos_state_compute(&view, &piv, &out) == 1);
    assert(fabs(out.bos_up    - 1.0) < 1e-12);
    assert(fabs(out.level     - 4.5) < 1e-12);
    assert(fabs(out.choch_up  - 1.0) < 1e-12);
    assert(fabs(out.bos_down  - 0.0) < 1e-12);
    assert(fabs(out.choch_down - 0.0) < 1e-12);
    printf("  ✓ test_bos_up_choch\n");
}

static void test_bos_down_continuation(void) {
    cxta_series_bar         buf;
    /* Bearish structure: LH + LL.  Close breaks below last_low=0.8. */
    /* Trend-continuation BOS downward — not a CHoCH.                  */
    cxta_struct_pivot_state piv  = make_bearish_pivot();
    cxta_struct_bos_state   out  = {0};
    cxta_series_bar_view    view = make_close_view(&buf, 0.7);

    assert(cxta_struct_bos_state_compute(&view, &piv, &out) == 1);
    assert(fabs(out.bos_down   - 1.0) < 1e-12);
    assert(fabs(out.level      - 0.8) < 1e-12);
    assert(fabs(out.choch_down - 0.0) < 1e-12); /* not a reversal */
    assert(fabs(out.bos_up     - 0.0) < 1e-12);
    assert(fabs(out.choch_up   - 0.0) < 1e-12);
    printf("  ✓ test_bos_down_continuation\n");
}

static void test_bos_incomplete_pivot_no_choch(void) {
    cxta_series_bar         buf;
    /* Only one pivot high/low known — no previous → trend unknown → no CHoCH. */
    cxta_struct_pivot_state piv   = {0};
    piv.high              = 5.0;
    piv.low               = 1.2;
    piv.has_last_high     = 1;
    piv.has_last_low      = 1;
    /* has_prev_high / has_prev_low remain 0 */
    cxta_struct_bos_state   out  = {0};
    cxta_series_bar_view    view = make_close_view(&buf, 5.5);

    assert(cxta_struct_bos_state_compute(&view, &piv, &out) == 1);
    assert(fabs(out.bos_up    - 1.0) < 1e-12);
    assert(fabs(out.choch_up  - 0.0) < 1e-12); /* no trend context */
    printf("  ✓ test_bos_incomplete_pivot_no_choch\n");
}

void cxta_test_bos(void) {
    test_bos_null_args();
    test_bos_no_break();
    test_bos_up_continuation();
    test_bos_down_choch();
    test_bos_up_choch();
    test_bos_down_continuation();
    test_bos_incomplete_pivot_no_choch();
}
