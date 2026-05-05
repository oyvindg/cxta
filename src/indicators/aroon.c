/**
 * @file aroon.c
 * @brief Aroon helpers.
 */

#include <cxta/indicators/aroon.h>
#include <cxta/ts/smoothing.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

static const cxta_field_descriptor cxta_aroon_fields[] = {
    {"up", offsetof(cxta_aroon_output, up), true},
    {"down", offsetof(cxta_aroon_output, down), true},
    {"oscillator", offsetof(cxta_aroon_output, oscillator), true},
};

static const cxta_plot_field_descriptor cxta_aroon_plot_fields[] = {
    {
        .field_name = "up",
        .auto_plot = true,
        .label = "Aroon Up",
        .pane = "aroon",
        .color = "#22c55e",
        .style = "line",
        .scale = "aroon",
        .hover_summary = "Bars-since-high strength line.",
        .hover_indication = "High values show recent highs; use against Aroon Down for trend direction.",
    },
    {
        .field_name = "down",
        .auto_plot = true,
        .label = "Aroon Down",
        .pane = "aroon",
        .color = "#ef4444",
        .style = "line",
        .scale = "aroon",
        .hover_summary = "Bars-since-low strength line.",
        .hover_indication = "High values show recent lows; use against Aroon Up for trend direction.",
    },
    {
        .field_name = "oscillator",
        .auto_plot = true,
        .label = "Aroon Oscillator",
        .pane = "aroon",
        .color = "#22d3ee",
        .style = "line",
        .scale = "aroon",
        .hover_summary = "Aroon Up minus Aroon Down.",
        .hover_indication = "Positive values favor bullish trend pressure; negative values favor bearish pressure.",
    },
};

const cxta_indicator_plot_descriptor cxta_aroon_plot_descriptor = {
    "aroon",
    NULL,
    cxta_aroon_plot_fields,
    CXTA_ARRAY_COUNT(cxta_aroon_plot_fields),
};

static int cxta_aroon_descriptor_period_arg(const double* args,
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

static void cxta_aroon_descriptor_eval(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs,
                                       void* out) {
    cxta_aroon_output value;
    memset(&value, 0, sizeof(value));
    value = cxta_aroon(view, cxta_aroon_descriptor_period_arg(args, nargs, 0u, 14));
    if (out) memcpy(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_aroon_descriptor = {
    "aroon",
    1,
    1,
    -1,
    -1,
    2,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_aroon_output),
    0u,
    cxta_aroon_fields,
    CXTA_ARRAY_COUNT(cxta_aroon_fields),
    NULL,
    cxta_aroon_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_aroon_params,
    CXTA_ARRAY_COUNT(cxta_aroon_params),
    "aroon",
    &cxta_aroon_plot_descriptor,
};

cxta_aroon_output cxta_aroon(const cxta_series_bar_view* view, int period) {
    cxta_aroon_output out = {0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t lookback = (p < idx) ? p : idx;
    const size_t start = idx - lookback;

    size_t hi_idx = start;
    size_t lo_idx = start;
    double hi = view->bars[start].high;
    double lo = view->bars[start].low;
    for (size_t i = start + 1; i <= idx; ++i) {
        if (view->bars[i].high >= hi) {
            hi = view->bars[i].high;
            hi_idx = i;
        }
        if (view->bars[i].low <= lo) {
            lo = view->bars[i].low;
            lo_idx = i;
        }
    }

    {
        const double denom = (lookback > 0) ? (double)lookback : 1.0;
        const double bars_since_high = (double)(idx - hi_idx);
        const double bars_since_low = (double)(idx - lo_idx);
        out.up = 100.0 * (denom - bars_since_high) / denom;
        out.down = 100.0 * (denom - bars_since_low) / denom;
        out.oscillator = out.up - out.down;
    }
    return out;
}
