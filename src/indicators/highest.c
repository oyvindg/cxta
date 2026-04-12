/**
 * @file highest.c
 * @brief Rolling maximum (highest) descriptor implementation.
 */

#include <cxta/indicators/highest.h>
#include <cxta/series/bar.h>
#include <cxta/ts/smoothing.h>

#include <limits.h>
#include <math.h>
#include <stddef.h>

static int cxta_highest_period_arg(const double* args, size_t nargs, int fallback) {
    double raw;

    if (!args || nargs == 0u) return cxta_ts_clamp_period(fallback);
    raw = args[0];
    if (!isfinite(raw)) return cxta_ts_clamp_period(fallback);
    if (raw >= (double)INT_MAX) return INT_MAX;
    if (raw <= (double)INT_MIN) return INT_MIN;
    return cxta_ts_clamp_period((int)llround(raw));
}

static double cxta_highest_eval(const cxta_series_bar_view* view,
                                const double* args,
                                size_t nargs) {
    size_t idx;
    size_t start;
    size_t window;
    size_t i;
    double value;

    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;
    idx = cxta_series_clamp_index(view->size, view->index);
    window = (size_t)cxta_highest_period_arg(args, nargs, 1);
    if (window > idx + 1u) window = idx + 1u;
    start = idx + 1u - window;
    value = view->bars[start].high;

    for (i = start + 1u; i <= idx; ++i) {
        if (view->bars[i].high > value) value = view->bars[i].high;
    }
    return value;
}

static double cxta_highest_eval_scalar_src(const cxta_series_scalar_view* source,
                                           const double* args,
                                           size_t nargs) {
    size_t idx;
    size_t start;
    size_t window;
    size_t i;
    double value;

    if (!source || !cxta_series_scalar_view_valid(source)) return 0.0;
    idx = cxta_series_clamp_index(source->size, source->index);
    window = (size_t)cxta_highest_period_arg(args, nargs, 1);
    if (window > idx + 1u) window = idx + 1u;
    start = idx + 1u - window;
    value = source->values[start];

    for (i = start + 1u; i <= idx; ++i) {
        if (source->values[i] > value) value = source->values[i];
    }
    return value;
}

const cxta_indicator_descriptor cxta_highest_descriptor = {
    "highest",
    1,
    1,
    1,
    1,
    -1,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_SCALAR_SOURCE,
    0u,
    0u,
    NULL,
    0u,
    cxta_highest_eval,
    NULL,
    cxta_highest_eval_scalar_src,
    NULL,
    NULL,
    NULL,
    cxta_highest_params,
    CXTA_ARRAY_COUNT(cxta_highest_params),
};
