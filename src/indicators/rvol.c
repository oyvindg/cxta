/**
 * @file rvol.c
 * @brief Realized volatility helpers.
 */

#include <cxta/indicators/rvol.h>
#include <cxta/ts/smoothing.h>
#include <math.h>

double cxta_rvol(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    if (idx < 1) return 0.0;
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t samples = (p < idx) ? p : idx;

    const size_t end = idx;
    const size_t start = end + 1 - samples;

    double sum_sq = 0.0;
    for (size_t i = start; i <= end; ++i) {
        const double prev = view->bars[i - 1].close;
        if (prev <= 0.0) continue;
        const double r = log(view->bars[i].close / prev);
        sum_sq += r * r;
    }
    return sqrt(sum_sq);
}
