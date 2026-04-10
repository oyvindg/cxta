/**
 * @file stddev.c
 * @brief Rolling standard deviation helpers.
 */

#include <limits.h>
#include <cxta/indicators/stddev.h>
#include <cxta/ts/smoothing.h>
#include <cxta/ts/rolling.h>
#include <math.h>

static int cxta_stddev_descriptor_period_arg(const double* args,
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

static double cxta_stddev_descriptor_eval(const cxta_series_bar_view* view,
                                          const double* args,
                                          size_t nargs) {
    return cxta_stddev(view, cxta_stddev_descriptor_period_arg(args, nargs, 0u, 20));
}

const cxta_indicator_descriptor cxta_stddev_descriptor = {
    "stddev",
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
    cxta_stddev_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_stddev_params,
    CXTA_ARRAY_COUNT(cxta_stddev_params),
};

double cxta_stddev(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t window = (p < (idx + 1)) ? p : (idx + 1);
    if (window < 2) return 0.0;
    const size_t start = idx + 1 - window;

    cxta_ts_rolling_state st;
    cxta_ts_rolling_reset(&st);
    for (size_t i = start; i <= idx; ++i) {
        cxta_ts_rolling_add(&st, view->bars[i].close);
    }
    return sqrt(cxta_ts_rolling_var(&st));
}
