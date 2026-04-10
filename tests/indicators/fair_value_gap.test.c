#include <cxta/cxta.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>

static int nearly_equal(double lhs, double rhs) {
    return fabs(lhs - rhs) <= 1e-9;
}

void cxta_test_fair_value_gap(void) {
    {
        const cxta_series_bar bars[] = {
            {0, 10.0, 10.5, 9.5, 10.1, 1000.0},
            {1, 10.1, 10.7, 9.8, 10.2, 1000.0},
            {2, 10.2, 10.8, 9.9, 10.3, 1000.0},
        };
        const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3u, 2u);
        const cxta_fair_value_gap_output out = cxta_fair_value_gap(&view);

        assert(nearly_equal(out.gap_high, 0.0));
        assert(nearly_equal(out.gap_low, 0.0));
        assert(nearly_equal(out.direction, 0.0));
        assert(nearly_equal(out.mitigated, 0.0));
        assert(nearly_equal(out.fill_pct, 0.0));
    }

    {
        const cxta_series_bar bars[] = {
            {0, 9.5, 10.0, 9.0, 9.8, 1000.0},
            {1, 10.5, 11.0, 10.0, 10.8, 1000.0},
            {2, 12.2, 13.0, 12.0, 12.5, 1000.0},
        };
        const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3u, 2u);
        const cxta_fair_value_gap_output out = cxta_fair_value_gap(&view);

        assert(nearly_equal(out.gap_low, 10.0));
        assert(nearly_equal(out.gap_high, 12.0));
        assert(nearly_equal(out.direction, 1.0));
        assert(nearly_equal(out.mitigated, 0.0));
        assert(nearly_equal(out.fill_pct, 0.0));
    }

    {
        const cxta_series_bar bars[] = {
            {0, 14.0, 14.5, 13.0, 13.8, 1000.0},
            {1, 13.0, 13.2, 12.0, 12.5, 1000.0},
            {2, 10.8, 11.0, 10.0, 10.5, 1000.0},
        };
        const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3u, 2u);
        const cxta_fair_value_gap_output out = cxta_fair_value_gap(&view);

        assert(nearly_equal(out.gap_low, 11.0));
        assert(nearly_equal(out.gap_high, 13.0));
        assert(nearly_equal(out.direction, -1.0));
        assert(nearly_equal(out.mitigated, 0.0));
        assert(nearly_equal(out.fill_pct, 0.0));
    }

    {
        const cxta_series_bar bars[] = {
            {0, 9.5, 10.0, 9.0, 9.8, 1000.0},
            {1, 10.5, 11.0, 10.0, 10.8, 1000.0},
            {2, 12.2, 13.0, 12.0, 12.5, 1000.0},
            {3, 11.4, 12.1, 9.6, 9.8, 1000.0},
        };
        const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 4u, 3u);
        const cxta_fair_value_gap_output out = cxta_fair_value_gap(&view);

        assert(nearly_equal(out.gap_low, 10.0));
        assert(nearly_equal(out.gap_high, 12.0));
        assert(nearly_equal(out.direction, 1.0));
        assert(nearly_equal(out.mitigated, 1.0));
        assert(nearly_equal(out.fill_pct, 1.0));
    }

    {
        const cxta_series_bar bars[] = {
            {0, 9.5, 10.0, 9.0, 9.8, 1000.0},
            {1, 10.5, 11.0, 10.0, 10.8, 1000.0},
            {2, 12.2, 13.0, 12.0, 12.5, 1000.0},
            {3, 11.4, 12.1, 10.8, 11.0, 1000.0},
            {4, 10.9, 12.4, 10.5, 10.8, 1000.0},
        };
        const cxta_series_bar_view mitigation_view = cxta_series_bar_view_make(bars, 5u, 3u);
        const cxta_series_bar_view after_view = cxta_series_bar_view_make(bars, 5u, 4u);
        const cxta_fair_value_gap_output mitigation = cxta_fair_value_gap(&mitigation_view);
        const cxta_fair_value_gap_output after = cxta_fair_value_gap(&after_view);

        assert(nearly_equal(mitigation.mitigated, 1.0));
        assert(nearly_equal(mitigation.fill_pct, 0.5));
        assert(nearly_equal(after.direction, 0.0));
        assert(nearly_equal(after.mitigated, 0.0));
        assert(nearly_equal(after.fill_pct, 0.0));
    }

    {
        const cxta_series_bar bars[] = {
            {0, 9.5, 10.0, 9.0, 9.8, 1000.0},
            {1, 10.5, 11.0, 10.0, 10.8, 1000.0},
            {2, 12.2, 13.0, 12.0, 12.5, 1000.0},
            {3, 11.0, 11.4, 10.8, 11.0, 1000.0},
            {4, 13.4, 14.0, 13.0, 13.5, 1000.0},
            {5, 14.1, 15.0, 14.0, 14.5, 1000.0},
            {6, 11.2, 12.0, 11.0, 11.5, 1000.0},
        };
        const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 7u, 6u);
        const cxta_fair_value_gap_output out = cxta_fair_value_gap(&view);

        assert(nearly_equal(out.gap_low, 12.0));
        assert(nearly_equal(out.gap_high, 13.0));
        assert(nearly_equal(out.direction, -1.0));
        assert(nearly_equal(out.mitigated, 0.0));
        assert(nearly_equal(out.fill_pct, 0.0));
    }

    printf("  ✓ test_fair_value_gap\n");
}
