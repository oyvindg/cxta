/**
 * @file stochrsi.c
 * @brief Stoch RSI helpers.
 */

#include <cxta/indicators/stochrsi.h>
#include <cxta/indicators/rsi.h>
#include <cxta/ts/smoothing.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

static const cxta_field_descriptor cxta_stoch_rsi_fields[] = {
    {"k", offsetof(cxta_stochrsi_output, k), true},
    {"d", offsetof(cxta_stochrsi_output, d), true},
};

static int cxta_stoch_rsi_descriptor_period_arg(const double* args,
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

static void cxta_stoch_rsi_descriptor_eval(const cxta_series_bar_view* view,
                                           const double* args,
                                           size_t nargs,
                                           void* out) {
    int rsi_period = cxta_stoch_rsi_descriptor_period_arg(args, nargs, 0u, 14);
    int stoch_period = cxta_stoch_rsi_descriptor_period_arg(args, nargs, 1u, rsi_period);
    int smooth_k = cxta_stoch_rsi_descriptor_period_arg(args, nargs, 2u, 3);
    int smooth_d = cxta_stoch_rsi_descriptor_period_arg(args, nargs, 3u, 3);
    cxta_stochrsi_output value = cxta_stochrsi(view, rsi_period, stoch_period, smooth_k, smooth_d);
    if (out) memcpy(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_stoch_rsi_descriptor = {
    "stoch_rsi",
    1,
    4,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_stochrsi_output),
    0u,
    cxta_stoch_rsi_fields,
    CXTA_ARRAY_COUNT(cxta_stoch_rsi_fields),
    NULL,
    cxta_stoch_rsi_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_stoch_rsi_params,
    CXTA_ARRAY_COUNT(cxta_stoch_rsi_params),
};

static double cxta_stochrsi_rsi_at(const cxta_series_bar_view* view, size_t idx, int period) {
    const cxta_series_bar_view sub = {view->bars, idx + 1, idx};
    return cxta_rsi(&sub, period);
}

cxta_stochrsi_output cxta_stochrsi(const cxta_series_bar_view* view,
                                   int rsi_period,
                                   int stoch_period,
                                   int smooth_k,
                                   int smooth_d) {
    cxta_stochrsi_output out = {0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const size_t stoch_p = (size_t)cxta_ts_clamp_period(stoch_period);
        const size_t sk = (size_t)cxta_ts_clamp_period(smooth_k);
        const size_t sd = (size_t)cxta_ts_clamp_period(smooth_d);
        const size_t d_window = (sd < (idx + 1)) ? sd : (idx + 1);
        const size_t d_start = idx + 1 - d_window;
        double d_sum = 0.0;

        for (size_t t = d_start; t <= idx; ++t) {
            const size_t k_window = (sk < (t + 1)) ? sk : (t + 1);
            const size_t k_start = t + 1 - k_window;
            double k_sum = 0.0;

            for (size_t j = k_start; j <= t; ++j) {
                const size_t r_window = (stoch_p < (j + 1)) ? stoch_p : (j + 1);
                const size_t r_start = j + 1 - r_window;
                double rsi_min = cxta_stochrsi_rsi_at(view, r_start, rsi_period);
                double rsi_max = rsi_min;
                const double rsi_now = cxta_stochrsi_rsi_at(view, j, rsi_period);

                for (size_t i = r_start + 1; i <= j; ++i) {
                    const double r = cxta_stochrsi_rsi_at(view, i, rsi_period);
                    if (r < rsi_min) rsi_min = r;
                    if (r > rsi_max) rsi_max = r;
                }

                {
                    const double denom = rsi_max - rsi_min;
                    const double stoch = (denom <= 1e-12)
                        ? 50.0
                        : (100.0 * (rsi_now - rsi_min) / denom);
                    k_sum += stoch;
                }
            }

            {
                const double k_value = k_sum / (double)k_window;
                if (t == idx) out.k = k_value;
                d_sum += k_value;
            }
        }
        out.d = d_sum / (double)d_window;
    }
    return out;
}
