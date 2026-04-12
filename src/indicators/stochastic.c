/**
 * @file stochastic.c
 * @brief Stochastic oscillator helpers.
 */

#include <cxta/indicators/stochastic.h>
#include <cxta/ts/smoothing.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

static const cxta_field_descriptor cxta_stochastic_fields[] = {
    {"k", offsetof(cxta_stoch_output, k), true},
    {"d", offsetof(cxta_stoch_output, d), true},
};

static int cxta_stochastic_descriptor_period_arg(const double* args,
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

static void cxta_stochastic_descriptor_eval(const cxta_series_bar_view* view,
                                            const double* args,
                                            size_t nargs,
                                            void* out) {
    const cxta_stoch_output value =
        cxta_stochastic(view,
                        cxta_stochastic_descriptor_period_arg(args, nargs, 0u, 14),
                        cxta_stochastic_descriptor_period_arg(args, nargs, 1u, 3),
                        cxta_stochastic_descriptor_period_arg(args, nargs, 2u, 3));
    if (out) memcpy(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_stochastic_descriptor = {
    "stochastic",
    1,
    3,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_stoch_output),
    0u,
    cxta_stochastic_fields,
    CXTA_ARRAY_COUNT(cxta_stochastic_fields),
    NULL,
    cxta_stochastic_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_stochastic_params,
    CXTA_ARRAY_COUNT(cxta_stochastic_params),
};

static double cxta_stochastic_raw_k(const cxta_series_bar_view* view, size_t idx, int k_period) {
    const size_t k = (size_t)cxta_ts_clamp_period(k_period);
    const size_t window = (k < (idx + 1)) ? k : (idx + 1);
    const size_t start = idx + 1 - window;
    double highest = view->bars[start].high;
    double lowest = view->bars[start].low;

    for (size_t i = start + 1; i <= idx; ++i) {
        if (view->bars[i].high > highest) highest = view->bars[i].high;
        if (view->bars[i].low < lowest) lowest = view->bars[i].low;
    }

    {
        const double range = highest - lowest;
        if (range <= 0.0) return 50.0;
        return 100.0 * (view->bars[idx].close - lowest) / range;
    }
}

static double cxta_stochastic_sma_raw_k(const cxta_series_bar_view* view,
                                        size_t idx,
                                        int k_period,
                                        int smooth_k) {
    const size_t s = (size_t)cxta_ts_clamp_period(smooth_k);
    const size_t window = (s < (idx + 1)) ? s : (idx + 1);
    const size_t start = idx + 1 - window;
    double sum = 0.0;

    for (size_t i = start; i <= idx; ++i) {
        sum += cxta_stochastic_raw_k(view, i, k_period);
    }
    return sum / (double)window;
}

cxta_stoch_output cxta_stochastic(const cxta_series_bar_view* view,
                                  int k_period,
                                  int d_period,
                                  int smooth_k) {
    cxta_stoch_output out = {0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const size_t d_win = (size_t)cxta_ts_clamp_period(d_period);
        const size_t window = (d_win < (idx + 1)) ? d_win : (idx + 1);
        const size_t start = idx + 1 - window;
        double d_sum = 0.0;

        out.k = cxta_stochastic_sma_raw_k(view, idx, k_period, smooth_k);
        for (size_t i = start; i <= idx; ++i) {
            d_sum += cxta_stochastic_sma_raw_k(view, i, k_period, smooth_k);
        }
        out.d = d_sum / (double)window;
    }
    return out;
}
