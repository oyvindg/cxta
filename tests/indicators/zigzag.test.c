#include <cxta/cxta.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>

static int cxta_zigzag_nearly_equal(double lhs, double rhs) {
    return fabs(lhs - rhs) <= 1e-9;
}

static cxta_series_bar cxta_zigzag_make_bar(double open, double high, double low, double close) {
    cxta_series_bar bar = {0, open, high, low, close, 100.0};
    return bar;
}

static void cxta_zigzag_test_empty_and_short_series_return_zero(void) {
    const cxta_zigzag_output out_empty = cxta_zigzag(NULL, 0.03, 0);
    assert(cxta_zigzag_nearly_equal(out_empty.high, 0.0));
    assert(cxta_zigzag_nearly_equal(out_empty.low, 0.0));

    {
        const cxta_series_bar bars[] = {
            {0, 100.0, 105.0, 98.0, 102.0, 100.0},
        };
        const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 1, 0);
        const cxta_zigzag_output out_one = cxta_zigzag(&view, 0.03, 0);
        assert(cxta_zigzag_nearly_equal(out_one.high, 0.0));
        assert(cxta_zigzag_nearly_equal(out_one.low, 0.0));
        assert(cxta_zigzag_nearly_equal(out_one.line, 0.0));
        assert(cxta_zigzag_nearly_equal(out_one.pivot_index, 0.0));
        assert(cxta_zigzag_nearly_equal(out_one.active, 0.0));
        assert(cxta_zigzag_nearly_equal(out_one.active_index, 0.0));
        assert(cxta_zigzag_nearly_equal(out_one.last, 0.0));
    }
}

static void cxta_zigzag_test_rising_then_falling_confirms_initial_low_and_top(void) {
    cxta_series_bar bars[10];
    for (int i = 0; i < 9; ++i) {
        const double c = 100.0 + (double)i * 10.0;
        bars[i] = cxta_zigzag_make_bar(c - 1.0, c + 2.0, c - 2.0, c);
    }
    bars[9] = cxta_zigzag_make_bar(178.0, 180.0, 150.0, 152.0);

    {
        const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 10, 9);
        const cxta_zigzag_output newest = cxta_zigzag(&view, 0.03, 0);
        const cxta_zigzag_output previous = cxta_zigzag(&view, 0.03, 1);

        assert(cxta_zigzag_nearly_equal(newest.high, 182.0));
        assert(cxta_zigzag_nearly_equal(newest.low, 98.0));
        assert(cxta_zigzag_nearly_equal(newest.line, 182.0));
        assert(cxta_zigzag_nearly_equal(newest.pivot_index, 8.0));
        assert(cxta_zigzag_nearly_equal(newest.active, 150.0));
        assert(cxta_zigzag_nearly_equal(newest.active_index, 9.0));
        assert(cxta_zigzag_nearly_equal(newest.last, 182.0));
        assert(cxta_zigzag_nearly_equal(newest.is_high, 1.0));
        assert(cxta_zigzag_nearly_equal(newest.direction, -1.0));

        assert(cxta_zigzag_nearly_equal(previous.last, 98.0));
        assert(cxta_zigzag_nearly_equal(previous.pivot_index, 0.0));
        assert(cxta_zigzag_nearly_equal(previous.is_high, 0.0));
    }
}

static void cxta_zigzag_test_ordered_pivot_history_and_same_type_history(void) {
    cxta_series_bar bars[19];
    for (int i = 0; i < 9; ++i) {
        const double c = 100.0 + (double)i * 10.0;
        bars[i] = cxta_zigzag_make_bar(c - 1.0, c + 2.0, c - 2.0, c);
    }
    bars[9] = cxta_zigzag_make_bar(178.0, 180.0, 150.0, 152.0);
    for (int i = 0; i < 8; ++i) {
        const double c = 155.0 + (double)i * 6.0;
        bars[10 + i] = cxta_zigzag_make_bar(c - 1.0, c + 2.0, c - 2.0, c);
    }
    bars[18] = cxta_zigzag_make_bar(190.0, 192.0, 155.0, 158.0);

    {
        const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 19, 18);
        const cxta_zigzag_output p0 = cxta_zigzag(&view, 0.03, 0);
        const cxta_zigzag_output p1 = cxta_zigzag(&view, 0.03, 1);
        const cxta_zigzag_output p2 = cxta_zigzag(&view, 0.03, 2);
        const cxta_zigzag_output p3 = cxta_zigzag(&view, 0.03, 3);

        assert(cxta_zigzag_nearly_equal(p0.last, 199.0));
        assert(cxta_zigzag_nearly_equal(p0.line, 199.0));
        assert(cxta_zigzag_nearly_equal(p0.pivot_index, 17.0));
        assert(cxta_zigzag_nearly_equal(p0.active, 155.0));
        assert(cxta_zigzag_nearly_equal(p0.active_index, 18.0));
        assert(cxta_zigzag_nearly_equal(p0.is_high, 1.0));
        assert(cxta_zigzag_nearly_equal(p1.last, 150.0));
        assert(cxta_zigzag_nearly_equal(p1.pivot_index, 9.0));
        assert(cxta_zigzag_nearly_equal(p1.is_high, 0.0));
        assert(cxta_zigzag_nearly_equal(p2.last, 182.0));
        assert(cxta_zigzag_nearly_equal(p2.pivot_index, 8.0));
        assert(cxta_zigzag_nearly_equal(p2.is_high, 1.0));
        assert(cxta_zigzag_nearly_equal(p3.last, 98.0));
        assert(cxta_zigzag_nearly_equal(p3.pivot_index, 0.0));
        assert(cxta_zigzag_nearly_equal(p3.is_high, 0.0));

        assert(cxta_zigzag_nearly_equal(p0.high, 199.0));
        assert(cxta_zigzag_nearly_equal(p1.high, 182.0));
        assert(cxta_zigzag_nearly_equal(p0.low, 150.0));
        assert(cxta_zigzag_nearly_equal(p1.low, 98.0));
    }
}

void cxta_test_zigzag(void) {
    cxta_zigzag_test_empty_and_short_series_return_zero();
    cxta_zigzag_test_rising_then_falling_confirms_initial_low_and_top();
    cxta_zigzag_test_ordered_pivot_history_and_same_type_history();
    printf("  ✓ test_zigzag\n");
}
