/**
 * @file trendline.c
 * @brief Pivot-derived trendline helpers.
 */

#include <cxta/indicators/trendline.h>
#include <cxta/ts/smoothing.h>

static double cxta_trendline_project_at(size_t x1, double y1, size_t x2, double y2, size_t x) {
    if (x2 <= x1) return y2;
    return y2 + ((y2 - y1) / (double)(x2 - x1)) * (double)(x - x2);
}

static double cxta_trendline_slope_between(size_t x1, double y1, size_t x2, double y2) {
    if (x2 <= x1) return 0.0;
    return (y2 - y1) / (double)(x2 - x1);
}

static int cxta_trendline_is_pivot_high(const cxta_series_bar_view* view,
                                        size_t index,
                                        int left,
                                        int right) {
    const double pivot = view->bars[index].high;
    const size_t begin = index - (size_t)left;
    const size_t end = index + (size_t)right;

    for (size_t i = begin; i <= end; ++i) {
        if (i == index) continue;
        if (pivot <= view->bars[i].high) return 0;
    }
    return 1;
}

static int cxta_trendline_is_pivot_low(const cxta_series_bar_view* view,
                                       size_t index,
                                       int left,
                                       int right) {
    const double pivot = view->bars[index].low;
    const size_t begin = index - (size_t)left;
    const size_t end = index + (size_t)right;

    for (size_t i = begin; i <= end; ++i) {
        if (i == index) continue;
        if (pivot >= view->bars[i].low) return 0;
    }
    return 1;
}

cxta_trendline_output cxta_trendline(const cxta_series_bar_view* view,
                                     int left,
                                     int right) {
    cxta_trendline_output out = {0.0, 0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    left = cxta_ts_clamp_period(left);
    right = cxta_ts_clamp_period(right);
    if (left < 1) left = 1;
    if (right < 1) right = 1;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        int has_high1 = 0;
        int has_high2 = 0;
        int has_low1 = 0;
        int has_low2 = 0;
        size_t hi1x = 0;
        size_t hi2x = 0;
        size_t lo1x = 0;
        size_t lo2x = 0;
        double hi1y = 0.0;
        double hi2y = 0.0;
        double lo1y = 0.0;
        double lo2y = 0.0;

        if (idx < (size_t)(left + right)) {
            out.upper = view->bars[idx].high;
            out.lower = view->bars[idx].low;
            return out;
        }

        for (size_t i = (size_t)left; i <= idx - (size_t)right; ++i) {
            if (cxta_trendline_is_pivot_high(view, i, left, right)) {
                has_high1 = has_high2;
                hi1x = hi2x;
                hi1y = hi2y;
                has_high2 = 1;
                hi2x = i;
                hi2y = view->bars[i].high;
            }
            if (cxta_trendline_is_pivot_low(view, i, left, right)) {
                has_low1 = has_low2;
                lo1x = lo2x;
                lo1y = lo2y;
                has_low2 = 1;
                lo2x = i;
                lo2y = view->bars[i].low;
            }
        }

        if (has_high1 && has_high2) {
            out.upper_slope = cxta_trendline_slope_between(hi1x, hi1y, hi2x, hi2y);
            out.upper = cxta_trendline_project_at(hi1x, hi1y, hi2x, hi2y, idx);
        } else if (has_high2) {
            out.upper = hi2y;
        } else {
            out.upper = view->bars[idx].high;
        }

        if (has_low1 && has_low2) {
            out.lower_slope = cxta_trendline_slope_between(lo1x, lo1y, lo2x, lo2y);
            out.lower = cxta_trendline_project_at(lo1x, lo1y, lo2x, lo2y, idx);
        } else if (has_low2) {
            out.lower = lo2y;
        } else {
            out.lower = view->bars[idx].low;
        }
    }
    return out;
}
