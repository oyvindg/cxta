/**
 * @file price_channel.c
 * @brief Price channel helpers.
 */

#include <cxta/indicators/channel.h>
#include <cxta/indicators/price_channel.h>
#include <cxta/indicators/donchian.h>
#include <cxta/ts/smoothing.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

static int cxta_price_channel_descriptor_period_arg(const double* args,
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

static void cxta_price_channel_descriptor_eval(const cxta_series_bar_view* view,
                                               const double* args,
                                               size_t nargs,
                                               void* out) {
    cxta_channel_output value;
    memset(&value, 0, sizeof(value));
    value = cxta_price_channel(
        view, cxta_price_channel_descriptor_period_arg(args, nargs, 0u, 20));
    if (out) memcpy(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_price_channel_descriptor = {
    "price_channel",
    1,
    1,
    -1,
    -1,
    2,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_channel_output),
    0u,
    cxta_channel_output_fields,
    CXTA_CHANNEL_OUTPUT_FIELD_COUNT,
    NULL,
    cxta_price_channel_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_price_channel_params,
    CXTA_ARRAY_COUNT(cxta_price_channel_params),
};

cxta_channel_output cxta_price_channel(const cxta_series_bar_view* view, int period) {
    return cxta_donchian(view, period);
}
