/**
 * @file keltner.c
 * @brief Keltner channel helpers.
 */

#include <cxta/indicators/channel.h>
#include <cxta/indicators/keltner.h>
#include <cxta/indicators/atr.h>
#include <cxta/indicators/ema.h>
#include <cxta/ts/smoothing.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

static int cxta_keltner_descriptor_int_arg(const double* args,
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

static int cxta_keltner_descriptor_period_arg(const double* args,
                                              size_t nargs,
                                              size_t index,
                                              int fallback) {
    return cxta_ts_clamp_period(cxta_keltner_descriptor_int_arg(args, nargs, index, fallback));
}

static double cxta_keltner_descriptor_double_arg(const double* args,
                                                 size_t nargs,
                                                 size_t index,
                                                 double fallback) {
    double raw;

    if (!args || index >= nargs) return fallback;
    raw = args[index];
    return isfinite(raw) ? raw : fallback;
}

static void cxta_keltner_descriptor_eval(const cxta_series_bar_view* view,
                                         const double* args,
                                         size_t nargs,
                                         void* out) {
    const int ema_period = cxta_keltner_descriptor_period_arg(args, nargs, 0u, 20);
    const double atr_mult = cxta_keltner_descriptor_double_arg(args, nargs, 1u, 2.0);
    const int atr_period = cxta_keltner_descriptor_period_arg(args, nargs, 2u, ema_period);
    cxta_channel_output value =
        cxta_keltner(view, ema_period, atr_mult, atr_period);
    if (out) memcpy(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_keltner_descriptor = {
    "keltner",
    2,
    3,
    -1,
    -1,
    2,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_channel_output),
    0u,
    cxta_channel_output_fields,
    CXTA_CHANNEL_OUTPUT_FIELD_COUNT,
    NULL,
    cxta_keltner_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_keltner_params,
    CXTA_ARRAY_COUNT(cxta_keltner_params),
};

cxta_channel_output cxta_keltner(const cxta_series_bar_view* view,
                                 int ema_period,
                                 double atr_mult,
                                 int atr_period) {
    cxta_channel_output out = {0.0, 0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    out.middle = cxta_ema(view, ema_period);
    {
        const double atr = cxta_atr(view, atr_period);
        out.upper = out.middle + (atr_mult * atr);
        out.lower = out.middle - (atr_mult * atr);
    }
    out.width = out.upper - out.lower;
    return out;
}
