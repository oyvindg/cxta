/**
 * @file window.c
 * @brief Rolling window helpers over cxta_series_bar_view.
 */

#include <cxta/series/window.h>

int cxta_series_window_highest_high(const cxta_series_bar_view* view, size_t start, size_t end, double* out) {
    if (!view || !out) return 0;
    if (!cxta_series_bar_view_valid(view) || start > end || end >= view->size) return 0;
    double v = view->bars[start].high;
    for (size_t i = start + 1; i <= end; ++i) {
        v = (v > view->bars[i].high) ? v : view->bars[i].high;
    }
    *out = v;
    return 1;
}

int cxta_series_window_lowest_low(const cxta_series_bar_view* view, size_t start, size_t end, double* out) {
    if (!view || !out) return 0;
    if (!cxta_series_bar_view_valid(view) || start > end || end >= view->size) return 0;
    double v = view->bars[start].low;
    for (size_t i = start + 1; i <= end; ++i) {
        v = (v < view->bars[i].low) ? v : view->bars[i].low;
    }
    *out = v;
    return 1;
}

double cxta_series_midpoint(double a, double b) {
    return 0.5 * (a + b);
}
