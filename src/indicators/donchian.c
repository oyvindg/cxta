/**
 * @file donchian.c
 * @brief Donchian channel helpers.
 */

#include <cxta/indicators/channel.h>
#include <cxta/indicators/donchian.h>
#include <cxta/indicators/extrema.h>
#include <cxta/indicators/macros.h>
#include <cxta/ts/smoothing.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

const cxta_field_descriptor cxta_channel_output_fields[] = {
    {"upper", offsetof(cxta_channel_output, upper), true},
    {"lower", offsetof(cxta_channel_output, lower), true},
    {"middle", offsetof(cxta_channel_output, middle), true},
    {"width", offsetof(cxta_channel_output, width), true},
};

static const cxta_plot_field_descriptor cxta_donchian_plot_fields[] = {
    {
        .field_name = "upper", .auto_plot = true, .label = "Donchian Upper",
        .pane = "donchian", .color = "#22c55e", .style = "line", .scale = "price",
        .show_price = true,
        .hover_summary = "Rolling highest high channel boundary.",
        .hover_indication = "Use as breakout resistance or trailing upper boundary.",
    },
    {
        .field_name = "lower", .auto_plot = true, .label = "Donchian Lower",
        .pane = "donchian", .color = "#ef4444", .style = "line", .scale = "price",
        .show_price = true,
        .hover_summary = "Rolling lowest low channel boundary.",
        .hover_indication = "Use as breakdown support or trailing lower boundary.",
    },
    {
        .field_name = "middle", .auto_plot = true, .label = "Donchian Middle",
        .pane = "donchian", .color = "#f59e0b", .style = "line", .scale = "price",
        .show_price = true,
        .hover_summary = "Midpoint between channel boundaries.",
        .hover_indication = "Use as mean/reference inside the channel.",
    },
    CXTA_FIELD_PLOT("width", false, "Donchian Width", "donchian_width", "#38bdf8", "line", "volatility", "Distance between upper and lower channel.", "Expansion indicates wider range; contraction indicates compression."),
};

static const cxta_indicator_plot_descriptor cxta_donchian_plot_descriptor = {
    .indicator_name = "donchian",
    .fields = cxta_donchian_plot_fields,
    .field_count = CXTA_ARRAY_COUNT(cxta_donchian_plot_fields),
};

static int cxta_donchian_descriptor_period_arg(const double* args,
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

static void cxta_donchian_descriptor_eval(const cxta_series_bar_view* view,
                                          const double* args,
                                          size_t nargs,
                                          void* out) {
    cxta_channel_output value;
    memset(&value, 0, sizeof(value));
    value = cxta_donchian(view, cxta_donchian_descriptor_period_arg(args, nargs, 0u, 20));
    if (out) memcpy(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_donchian_descriptor = {
    "donchian",
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
    cxta_donchian_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_donchian_params,
    CXTA_ARRAY_COUNT(cxta_donchian_params),
    "price",
    &cxta_donchian_plot_descriptor,
};

cxta_channel_output cxta_donchian(const cxta_series_bar_view* view, int period) {
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
