/**
 * @file pivot_points.c
 * @brief Classic floor-trader pivot points from previous UTC session OHLC.
 */

#include <cxta/structure/pivot_points.h>

#include <stddef.h>

int cxta_struct_pivot_points_compute(const cxta_series_bar_view* view,
                                     cxta_struct_pivot_points_result* out) {
    size_t current_start;
    size_t prev_start;
    size_t prev_last;
    double prev_high;
    double prev_low;
    double prev_close;
    double range;
    size_t i;

    if (!out) return 0;

    out->pp = 0.0;
    out->r1 = 0.0;
    out->r2 = 0.0;
    out->r3 = 0.0;
    out->s1 = 0.0;
    out->s2 = 0.0;
    out->s3 = 0.0;

    if (!cxta_series_bar_view_valid(view)) return 1;

    current_start = cxta_series_session_start_index(view->bars, view->size, view->index);
    if (current_start == 0u) return 1;

    prev_last = current_start - 1u;
    prev_start = cxta_series_session_start_index(view->bars, view->size, prev_last);

    prev_high = view->bars[prev_last].high;
    prev_low = view->bars[prev_last].low;
    prev_close = view->bars[prev_last].close;

    for (i = prev_start; i < prev_last; ++i) {
        const cxta_series_bar* bar = &view->bars[i];
        if (bar->high > prev_high) prev_high = bar->high;
        if (bar->low < prev_low) prev_low = bar->low;
    }

    out->pp = (prev_high + prev_low + prev_close) / 3.0;
    out->r1 = 2.0 * out->pp - prev_low;
    out->s1 = 2.0 * out->pp - prev_high;
    range = prev_high - prev_low;
    out->r2 = out->pp + range;
    out->s2 = out->pp - range;
    out->r3 = prev_high + 2.0 * (out->pp - prev_low);
    out->s3 = prev_low - 2.0 * (prev_high - out->pp);
    return 1;
}
