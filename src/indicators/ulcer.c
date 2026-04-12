/**
 * @file ulcer.c
 * @brief Ulcer Index helpers.
 */

#include <limits.h>
#include <math.h>

#include <cxta/indicators/ulcer.h>
#include <cxta/ts/smoothing.h>

static int cxta_ulcer_index_descriptor_period_arg(const double* args,
                                                  size_t nargs,
                                                  size_t index,
                                                  int fallback) {
    double raw;

    if (!args || index >= nargs) return fallback;
    raw = args[index];
    if (!isfinite(raw)) return fallback;
    if (raw >= (double)INT_MAX) return INT_MAX;
    if (raw <= (double)INT_MIN) return INT_MIN;
    return cxta_ts_clamp_period((int)llround(raw));
}

static double cxta_ulcer_index_descriptor_eval(const cxta_series_bar_view* view,
                                               const double* args,
                                               size_t nargs) {
    return cxta_ulcer(
        view, cxta_ulcer_index_descriptor_period_arg(args, nargs, 0u, 14));
}

const cxta_indicator_descriptor cxta_ulcer_index_descriptor = {
    "ulcer_index",
    1,
    1,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    0u,
    NULL,
    0u,
    cxta_ulcer_index_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_ulcer_index_params,
    CXTA_ARRAY_COUNT(cxta_ulcer_index_params),
};

double cxta_ulcer(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const size_t p = (size_t)cxta_ts_clamp_period(period);
        const size_t window = (p < (idx + 1)) ? p : (idx + 1);
        const size_t start = idx + 1 - window;
        double peak = view->bars[start].close;
        double sum_sq = 0.0;

        for (size_t i = start + 1; i <= idx; ++i) {
            if (view->bars[i].close > peak) peak = view->bars[i].close;
        }
        if (peak <= 0.0) return 0.0;

        for (size_t i = start; i <= idx; ++i) {
            const double dd = 100.0 * (view->bars[i].close - peak) / peak;
            sum_sq += dd * dd;
        }
        return sqrt(sum_sq / (double)window);
    }
}
