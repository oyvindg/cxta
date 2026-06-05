/**
 * @file adx.c
 * @brief ADX helpers.
 */

#include <cxta/indicators/adx.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

static const cxta_field_descriptor cxta_adx_fields[] = {
    {"adx", offsetof(cxta_adx_output, adx), false},
    {"plusDI", offsetof(cxta_adx_output, plus_di), false},
    {"minusDI", offsetof(cxta_adx_output, minus_di), false},
};

static const cxta_plot_field_descriptor cxta_adx_plot_fields[] = {
    {
        .field_name = "adx",
        .auto_plot = true,
        .label = "ADX",
        .pane = "adx",
        .color = "#f97316",
        .style = "line",
        .scale = "adx",
        .hover_summary = "Average Directional Index trend-strength line.",
        .hover_indication = "Rising ADX indicates stronger trend conditions; use with +DI/-DI for direction.",
    },
    {
        .field_name = "plusDI",
        .auto_plot = true,
        .label = "+DI",
        .pane = "adx",
        .color = "#22c55e",
        .style = "line",
        .scale = "adx",
        .hover_summary = "Positive Directional Indicator component.",
        .hover_indication = "A +DI lead over -DI favors bullish directional pressure.",
    },
    {
        .field_name = "minusDI",
        .auto_plot = true,
        .label = "-DI",
        .pane = "adx",
        .color = "#ef4444",
        .style = "line",
        .scale = "adx",
        .hover_summary = "Negative Directional Indicator component.",
        .hover_indication = "A -DI lead over +DI favors bearish directional pressure.",
    },
};

const cxta_indicator_plot_descriptor cxta_adx_plot_descriptor = {
    "adx",
    NULL,
    cxta_adx_plot_fields,
    CXTA_ARRAY_COUNT(cxta_adx_plot_fields),
};

static int cxta_adx_descriptor_int_arg(const double* args,
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

static int cxta_adx_descriptor_period_arg(const double* args,
                                          size_t nargs,
                                          size_t index,
                                          int fallback) {
    return cxta_adx_math_clamp_period(
        cxta_adx_descriptor_int_arg(args, nargs, index, fallback));
}

static void cxta_adx_descriptor_eval(const cxta_series_bar_view* view,
                                     const double* args,
                                     size_t nargs,
                                     void* out) {
    cxta_adx_output value;
    memset(&value, 0, sizeof(value));
    value = cxta_adx(view, cxta_adx_descriptor_period_arg(args, nargs, 0u, 14));
    if (out) memcpy(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_adx_descriptor = {
    "adx",
    1,
    1,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_adx_output),
    sizeof(cxta_adx_state),
    cxta_adx_fields,
    CXTA_ARRAY_COUNT(cxta_adx_fields),
    NULL,
    cxta_adx_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_adx_params,
    CXTA_ARRAY_COUNT(cxta_adx_params),
    "adx",
    &cxta_adx_plot_descriptor,
};

cxta_adx_output cxta_adx_step(double plus_dm,
                              double minus_dm,
                              double tr,
                              int period,
                              cxta_adx_state* st) {
    return cxta_adx_math_step(plus_dm, minus_dm, tr, period, st);
}

cxta_adx_output cxta_adx(const cxta_series_bar_view* view, int period) {
    cxta_adx_output out = {0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view) || view->index == 0) return out;

    const int p = cxta_adx_math_clamp_period(period);
    cxta_adx_state st = cxta_adx_math_init();
    for (size_t i = 1; i <= view->index; ++i) {
        out = cxta_adx_math_step_bar(
            view->bars[i].high,
            view->bars[i].low,
            view->bars[i - 1].high,
            view->bars[i - 1].low,
            view->bars[i - 1].close,
            p,
            &st);
    }
    return out;
}
