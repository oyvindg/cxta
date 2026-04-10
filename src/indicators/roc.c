/**
 * @file roc.c
 * @brief Rate of Change helpers.
 */

#include <limits.h>
#include <math.h>

#include <cxta/indicators/roc.h>
#include <cxta/ts/smoothing.h>

static int cxta_roc_descriptor_period_arg(const double* args,
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

static double cxta_roc_descriptor_eval(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs) {
    return cxta_roc(view, cxta_roc_descriptor_period_arg(args, nargs, 0u, 10));
}

const cxta_indicator_descriptor cxta_roc_descriptor = {
    "roc",
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
    cxta_roc_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_roc_params,
    CXTA_ARRAY_COUNT(cxta_roc_params),
};

double cxta_roc(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t ref = (p <= idx) ? (idx - p) : 0;
    const double ref_close = view->bars[ref].close;
    if (ref_close == 0.0) return 0.0;
    return (view->bars[idx].close / ref_close - 1.0) * 100.0;
}
