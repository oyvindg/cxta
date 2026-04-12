/**
 * @file ttm_squeeze.c
 * @brief TTM squeeze helpers.
 */

#include <cxta/indicators/ttm_squeeze.h>
#include <cxta/indicators/bollinger.h>
#include <cxta/indicators/keltner.h>
#include <cxta/indicators/sma.h>
#include <cxta/ts/smoothing.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

static const cxta_field_descriptor cxta_ttm_squeeze_fields[] = {
    {"squeeze_on", offsetof(cxta_squeeze_output, squeeze_on), true},
    {"squeeze_off", offsetof(cxta_squeeze_output, squeeze_off), true},
    {"momentum", offsetof(cxta_squeeze_output, momentum), true},
};

static int cxta_ttm_squeeze_descriptor_int_arg(const double* args,
                                               size_t nargs,
                                               size_t index,
                                               int fallback) {
    double raw;

    if (!args || index >= nargs) return fallback;
    raw = args[index];
    if (!isfinite(raw)) return fallback;
    if (raw >= (double)INT_MAX) return INT_MAX;
    if (raw <= (double)INT_MIN) return INT_MIN;
    return (int)llround(raw);
}

static int cxta_ttm_squeeze_descriptor_period_arg(const double* args,
                                                size_t nargs,
                                                size_t index,
                                                int fallback) {
    return cxta_ts_clamp_period(cxta_ttm_squeeze_descriptor_int_arg(args, nargs, index, fallback));
}

static double cxta_ttm_squeeze_descriptor_double_arg(const double* args,
                                                     size_t nargs,
                                                     size_t index,
                                                     double fallback) {
    double raw;

    if (!args || index >= nargs) return fallback;
    raw = args[index];
    return isfinite(raw) ? raw : fallback;
}

static void cxta_ttm_squeeze_descriptor_eval(const cxta_series_bar_view* view,
                                             const double* args,
                                             size_t nargs,
                                             void* out) {
    const cxta_squeeze_output value =
        cxta_ttm_squeeze(view,
                         cxta_ttm_squeeze_descriptor_period_arg(args, nargs, 0u, 20),
                         cxta_ttm_squeeze_descriptor_double_arg(args, nargs, 1u, 2.0),
                         cxta_ttm_squeeze_descriptor_period_arg(args, nargs, 2u, 20),
                         cxta_ttm_squeeze_descriptor_double_arg(args, nargs, 3u, 1.5),
                         cxta_ttm_squeeze_descriptor_period_arg(args, nargs, 4u, 20));
    if (out) memcpy(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_ttm_squeeze_descriptor = {
    "ttm_squeeze",
    0,
    5,
    -1,
    -1,
    2,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_squeeze_output),
    0u,
    cxta_ttm_squeeze_fields,
    CXTA_ARRAY_COUNT(cxta_ttm_squeeze_fields),
    NULL,
    cxta_ttm_squeeze_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_ttm_squeeze_params,
    CXTA_ARRAY_COUNT(cxta_ttm_squeeze_params),
};

cxta_squeeze_output cxta_ttm_squeeze(const cxta_series_bar_view* view,
                                     int bb_period,
                                     double bb_mult,
                                     int kc_period,
                                     double kc_mult,
                                     int momentum_period) {
    cxta_squeeze_output out = {0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    {
        const cxta_bollinger_output bb = cxta_bollinger(view, bb_period, bb_mult);
        const cxta_channel_output kc = cxta_keltner(view, kc_period, kc_mult, kc_period);
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const double momentum_mean = cxta_sma(view, momentum_period);

        out.squeeze_on = (bb.upper < kc.upper && bb.lower > kc.lower) ? 1.0 : 0.0;
        out.squeeze_off = (bb.upper > kc.upper && bb.lower < kc.lower) ? 1.0 : 0.0;
        out.momentum = view->bars[idx].close - momentum_mean;
    }
    return out;
}
