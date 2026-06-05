/**
 * @file bollinger.c
 * @brief Bollinger band helpers.
 */

#include <limits.h>
#include <math.h>
#include <string.h>

#include <cxta/indicators/bollinger.h>
#include <cxta/ts/smoothing.h>

static const cxta_field_descriptor cxta_bollinger_fields[] = {
    {"upper", offsetof(cxta_bollinger_output, upper), true},
    {"lower", offsetof(cxta_bollinger_output, lower), true},
    {"middle", offsetof(cxta_bollinger_output, middle), true},
    {"percentB", offsetof(cxta_bollinger_output, percent_b), true},
    {"width", offsetof(cxta_bollinger_output, bandwidth), true},
};

static const cxta_plot_field_descriptor cxta_bollinger_plot_fields[] = {
    {
        .field_name = "upper",
        .auto_plot = true,
        .label = "Upper Band",
        .pane = "price",
        .color = "#60a5fa",
        .style = "line",
        .scale = "price",
        .hover_summary = "Upper Bollinger band.",
        .hover_indication = "Touches or breaks can indicate extension; compare with middle band and bandwidth.",
    },
    {
        .field_name = "lower",
        .auto_plot = true,
        .label = "Lower Band",
        .pane = "price",
        .color = "#60a5fa",
        .style = "line",
        .scale = "price",
        .hover_summary = "Lower Bollinger band.",
        .hover_indication = "Touches or breaks can indicate downside extension; compare with middle band and bandwidth.",
    },
    {
        .field_name = "middle",
        .auto_plot = true,
        .label = "Middle Band",
        .pane = "price",
        .color = "#f59e0b",
        .style = "line",
        .scale = "price",
        .hover_summary = "Middle moving-average band.",
        .hover_indication = "Often used as mean reference inside the Bollinger envelope.",
    },
    {
        .field_name = "percentB",
        .auto_plot = false,
        .label = "Percent B",
        .pane = "price",
        .color = "#a78bfa",
        .style = "line",
        .scale = "price",
        .hover_summary = "Relative position inside the Bollinger envelope.",
        .hover_indication = "Values below 0 or above 1 indicate price outside the lower or upper band.",
    },
    {
        .field_name = "width",
        .auto_plot = false,
        .label = "Band Width",
        .pane = "price",
        .color = "#34d399",
        .style = "line",
        .scale = "price",
        .hover_summary = "Width of the Bollinger envelope.",
        .hover_indication = "Contracting width suggests compression; expansion suggests volatility release.",
    },
};

const cxta_indicator_plot_descriptor cxta_bollinger_plot_descriptor = {
    "bollinger",
    NULL,
    cxta_bollinger_plot_fields,
    CXTA_ARRAY_COUNT(cxta_bollinger_plot_fields),
};

static int cxta_bollinger_descriptor_period_arg(const double* args,
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

static double cxta_bollinger_descriptor_double_arg(const double* args,
                                                   size_t nargs,
                                                   size_t index,
                                                   double fallback) {
    double raw;

    if (!args || index >= nargs) return fallback;
    raw = args[index];
    return isfinite(raw) ? raw : fallback;
}

static void cxta_bollinger_descriptor_copy_struct(void* out,
                                                  const void* value,
                                                  size_t size) {
    if (!out || !value || size == 0u) return;
    memcpy(out, value, size);
}

static void cxta_bollinger_descriptor_eval(const cxta_series_bar_view* view,
                                           const double* args,
                                           size_t nargs,
                                           void* out) {
    cxta_bollinger_output value;

    memset(&value, 0, sizeof(value));
    value = cxta_bollinger(
        view,
        cxta_bollinger_descriptor_period_arg(args, nargs, 0u, 20),
        cxta_bollinger_descriptor_double_arg(args, nargs, 1u, 2.0));
    cxta_bollinger_descriptor_copy_struct(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_bollinger_descriptor = {
    "bollinger",
    2,
    2,
    -1,
    -1,
    2,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_bollinger_output),
    0u,
    cxta_bollinger_fields,
    CXTA_ARRAY_COUNT(cxta_bollinger_fields),
    NULL,
    cxta_bollinger_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_bollinger_params,
    CXTA_ARRAY_COUNT(cxta_bollinger_params),
    "price",
    &cxta_bollinger_plot_descriptor,
};

cxta_bollinger_output cxta_bollinger(const cxta_series_bar_view* view,
                                     int period,
                                     double stddev_mult) {
    cxta_bollinger_output out = {0.0, 0.0, 0.0, 0.0, 0.0};
    size_t idx;

    if (!view || !cxta_series_bar_view_valid(view)) return out;

    idx = cxta_series_clamp_index(view->size, view->index);
    return cxta_bollinger_math_eval_rows(
        view->bars,
        (unsigned long long)view->size,
        (unsigned long long)idx,
        (unsigned long long)sizeof(view->bars[0]),
        (unsigned long long)offsetof(cxta_series_bar, close),
        cxta_ts_clamp_period(period),
        stddev_mult);
}
