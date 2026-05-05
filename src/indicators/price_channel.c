/**
 * @file price_channel.c
 * @brief Price channel helpers.
 */

#include <cxta/indicators/channel.h>
#include <cxta/indicators/extrema.h>
#include <cxta/indicators/price_channel.h>
#include <cxta/indicators/macros.h>
#include <cxta/ts/smoothing.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

static const cxta_plot_field_descriptor cxta_price_channel_plot_fields[] = {
    CXTA_FIELD_PLOT("upper", true, "Price Channel Upper", "price", "#22c55e", "line", "price", "Rolling upper price channel boundary.", "Use as breakout resistance or trailing upper boundary."),
    CXTA_FIELD_PLOT("lower", true, "Price Channel Lower", "price", "#ef4444", "line", "price", "Rolling lower price channel boundary.", "Use as breakdown support or trailing lower boundary."),
    CXTA_FIELD_PLOT("middle", true, "Price Channel Middle", "price", "#f59e0b", "line", "price", "Midpoint between price channel boundaries.", "Use as channel mean/reference."),
    CXTA_FIELD_PLOT("width", false, "Price Channel Width", "channel", "#38bdf8", "line", "channel", "Distance between upper and lower channel.", "Expansion indicates wider range; contraction indicates compression."),
};

static const cxta_indicator_plot_descriptor cxta_price_channel_plot_descriptor = {
    .indicator_name = "price_channel",
    .fields = cxta_price_channel_plot_fields,
    .field_count = CXTA_ARRAY_COUNT(cxta_price_channel_plot_fields),
};

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
    "price",
    &cxta_price_channel_plot_descriptor,
};

cxta_channel_output cxta_price_channel(const cxta_series_bar_view* view, int period) {
    cxta_channel_output out = {0.0, 0.0, 0.0, 0.0};
    cxta_extrema_output extrema;

    if (!view || !cxta_series_bar_view_valid(view)) return out;

    extrema = cxta_extrema(view, period);
    out.upper = extrema.high;
    out.lower = extrema.low;
    out.middle = extrema.mid;
    out.width = out.upper - out.lower;
    return out;
}
