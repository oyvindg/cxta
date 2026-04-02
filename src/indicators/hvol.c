/**
 * @file hvol.c
 * @brief Historical volatility helpers.
 */

#include <cxta/indicators/hvol.h>
#include <cxta/ts/smoothing.h>
#include <math.h>

double cxta_hvol(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    /* We need p log-returns, which requires p+1 bars */
    if (idx < 1) return 0.0;
    const size_t samples = (p < idx) ? p : idx;
    if (samples < 1) return 0.0;

    const size_t end = idx;
    const size_t start = end + 1 - samples; /* start of the window (bar after reference) */

    double sum = 0.0, sum_sq = 0.0;
    for (size_t i = start; i <= end; ++i) {
        const double prev = view->bars[i - 1].close;
        if (prev <= 0.0) continue;
        const double r = log(view->bars[i].close / prev);
        sum += r;
        sum_sq += r * r;
    }
    const double n = (double)samples;
    const double var = sum_sq / n - (sum / n) * (sum / n);
    return (var > 0.0) ? (sqrt(var) * sqrt(252.0)) : 0.0;
}
