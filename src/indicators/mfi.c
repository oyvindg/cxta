/**
 * @file mfi.c
 * @brief Money Flow Index helpers.
 */

#include <limits.h>
#include <math.h>

#include <cxta/indicators/mfi.h>
#include <cxta/ts/smoothing.h>

static int cxta_mfi_descriptor_period_arg(const double* args,
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

static double cxta_mfi_descriptor_eval(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs) {
    return cxta_mfi(view, cxta_mfi_descriptor_period_arg(args, nargs, 0u, 14));
}

const cxta_indicator_descriptor cxta_mfi_descriptor = {
    "mfi",
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
    cxta_mfi_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_mfi_params,
    CXTA_ARRAY_COUNT(cxta_mfi_params),
};

double cxta_mfi(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    if (idx == 0) return 50.0;

    {
        const size_t p = (size_t)cxta_ts_clamp_period(period);
        const size_t start = (idx >= p) ? (idx - p + 1) : 1;
        double positive_flow = 0.0;
        double negative_flow = 0.0;

        for (size_t i = start; i <= idx; ++i) {
            const double tp_prev = cxta_series_typical_price(&view->bars[i - 1]);
            const double tp_curr = cxta_series_typical_price(&view->bars[i]);
            const double money_flow = tp_curr * view->bars[i].volume;
            if (tp_curr > tp_prev) positive_flow += money_flow;
            else if (tp_curr < tp_prev) negative_flow += money_flow;
        }

        if (positive_flow <= 1e-12 && negative_flow <= 1e-12) return 50.0;
        if (negative_flow <= 1e-12) return 100.0;

        {
            const double ratio = positive_flow / negative_flow;
            return 100.0 - (100.0 / (1.0 + ratio));
        }
    }
}
