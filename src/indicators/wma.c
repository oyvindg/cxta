/**
 * @file wma.c
 * @brief Weighted moving average helpers.
 */

#include <limits.h>
#include <math.h>

#include <cxta/indicators/wma.h>
#include <cxta/indicators/macros.h>
#include <cxta/ts/smoothing.h>

static const cxta_scalar_plot_descriptor cxta_wma_scalar_plot =
    CXTA_SCALAR_PLOT("WMA", "price", "#38bdf8", "line", "price",
                     "Weighted Moving Average price overlay.",
                     "Emphasizes recent closes; compare slope and crosses against price or slower averages.");

static const cxta_indicator_plot_descriptor cxta_wma_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("wma", cxta_wma_scalar_plot);

static int cxta_wma_descriptor_period_arg(const double* args,
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

static double cxta_wma_descriptor_eval(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs) {
    return cxta_wma(view, cxta_wma_descriptor_period_arg(args, nargs, 0u, 20));
}

const cxta_indicator_descriptor cxta_wma_descriptor = {
    "wma",
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
    cxta_wma_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_wma_params,
    CXTA_ARRAY_COUNT(cxta_wma_params),
    "price",
    &cxta_wma_plot_descriptor,
};

double cxta_wma(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = view->index;
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t window = (p < (idx + 1)) ? p : (idx + 1);
    const size_t start = idx + 1 - window;

    double weighted_sum = 0.0;
    double weight_sum = 0.0;
    double weight = 1.0;
    for (size_t i = start; i <= idx; ++i, weight += 1.0) {
        weighted_sum += view->bars[i].close * weight;
        weight_sum += weight;
    }
    return (weight_sum > 0.0) ? (weighted_sum / weight_sum) : 0.0;
}
