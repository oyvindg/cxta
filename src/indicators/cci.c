/**
 * @file cci.c
 * @brief Commodity Channel Index helpers.
 */

#include <limits.h>
#include <math.h>

#include <cxta/indicators/cci.h>
#include <cxta/ts/smoothing.h>

static int cxta_cci_descriptor_period_arg(const double* args,
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

static double cxta_cci_descriptor_eval(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs) {
    return cxta_cci(view, cxta_cci_descriptor_period_arg(args, nargs, 0u, 20));
}

const cxta_indicator_descriptor cxta_cci_descriptor = {
    "cci",
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
    cxta_cci_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_cci_params,
    CXTA_ARRAY_COUNT(cxta_cci_params),
};

double cxta_cci(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t window = (p < (idx + 1)) ? p : (idx + 1);
    const size_t start = idx + 1 - window;

    double tp_sum = 0.0;
    for (size_t i = start; i <= idx; ++i) {
        tp_sum += cxta_series_typical_price(&view->bars[i]);
    }

    {
        const double sma_tp = tp_sum / (double)window;
        double mean_dev_sum = 0.0;
        for (size_t i = start; i <= idx; ++i) {
            mean_dev_sum += fabs(cxta_series_typical_price(&view->bars[i]) - sma_tp);
        }

        {
            const double mean_dev = mean_dev_sum / (double)window;
            if (mean_dev <= 0.0) return 0.0;
            return (cxta_series_typical_price(&view->bars[idx]) - sma_tp) / (0.015 * mean_dev);
        }
    }
}
