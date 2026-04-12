/**
 * @file hvol.c
 * @brief Historical volatility helpers.
 */

#include <cxta/indicators/macros.h>
#include <cxta/indicators/hvol.h>
#include <cxta/ts/smoothing.h>
#include <limits.h>
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

static double cxta_historical_volatility_desc_eval(const cxta_series_bar_view* view,
                                                   const double* args,
                                                   size_t nargs) {
    const int period = cxta_descriptor_period_arg(args, nargs, 0u, 20);
    const int annualization =
        cxta_descriptor_clamp_int_arg(args, nargs, 1u, 252, 1, INT_MAX);
    const double base = cxta_hvol(view, period);
    if (annualization == 252) return base;
    return base * sqrt((double)annualization / 252.0);
}

const cxta_indicator_descriptor cxta_historical_volatility_descriptor = {
    "historical_volatility",
    1,
    2,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    0u,
    NULL,
    0u,
    cxta_historical_volatility_desc_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_historical_volatility_params,
    CXTA_ARRAY_COUNT(cxta_historical_volatility_params),
};
