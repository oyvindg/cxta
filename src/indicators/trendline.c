/**
 * @file trendline.c
 * @brief Pivot-derived trendline helpers.
 */

#include <cxta/indicators/trendline.h>
#include <cxta/indicators/macros.h>
#include <cxta/ts/smoothing.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

static const cxta_field_descriptor cxta_trendline_fields[] = {
    {"upper", offsetof(cxta_trendline_output, upper), true},
    {"lower", offsetof(cxta_trendline_output, lower), true},
    {"upperSlope", offsetof(cxta_trendline_output, upper_slope), true},
    {"lowerSlope", offsetof(cxta_trendline_output, lower_slope), true},
    {"upperPivot", offsetof(cxta_trendline_output, upper_pivot), false},
    {"upperPivotIndex", offsetof(cxta_trendline_output, upper_pivot_index), false},
    {"lowerPivot", offsetof(cxta_trendline_output, lower_pivot), false},
    {"lowerPivotIndex", offsetof(cxta_trendline_output, lower_pivot_index), false},
    {"activeIndex", offsetof(cxta_trendline_output, active_index), false},
};

static const cxta_plot_field_descriptor cxta_trendline_plot_fields[] = {
    {
        .field_name = "upper",
        .auto_plot = true,
        .label = "Upper Trendline",
        .pane = "price",
        .color = "#22c55e",
        .style = "zigzag",
        .scale = "price",
        .pivot_value_field = "upperPivot",
        .pivot_index_field = "upperPivotIndex",
        .active_value_field = "upper",
        .active_index_field = "activeIndex",
        .pivot_zero_is_missing = true,
        .active_zero_is_missing = true,
        .hover_summary = "Projected upper pivot trendline.",
        .hover_indication = "Use as dynamic resistance and breakout reference.",
    },
    {
        .field_name = "lower",
        .auto_plot = true,
        .label = "Lower Trendline",
        .pane = "price",
        .color = "#ef4444",
        .style = "zigzag",
        .scale = "price",
        .pivot_value_field = "lowerPivot",
        .pivot_index_field = "lowerPivotIndex",
        .active_value_field = "lower",
        .active_index_field = "activeIndex",
        .pivot_zero_is_missing = true,
        .active_zero_is_missing = true,
        .hover_summary = "Projected lower pivot trendline.",
        .hover_indication = "Use as dynamic support and breakdown reference.",
    },
    CXTA_FIELD_PLOT("upperSlope", false, "Upper Slope", "trendline", "#22c55e", "line", "trendline", "Slope of the upper trendline.", "Use slope sign/magnitude to classify trendline pressure."),
    CXTA_FIELD_PLOT("lowerSlope", false, "Lower Slope", "trendline", "#ef4444", "line", "trendline", "Slope of the lower trendline.", "Use slope sign/magnitude to classify trendline pressure."),
};

static const cxta_indicator_plot_descriptor cxta_trendline_plot_descriptor = {
    .indicator_name = "trendline",
    .fields = cxta_trendline_plot_fields,
    .field_count = CXTA_ARRAY_COUNT(cxta_trendline_plot_fields),
};

static int cxta_trendline_descriptor_period_arg(const double* args,
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

static void cxta_trendline_descriptor_eval(const cxta_series_bar_view* view,
                                           const double* args,
                                           size_t nargs,
                                           void* out) {
    cxta_trendline_output value;
    memset(&value, 0, sizeof(value));
    value = cxta_trendline(view,
                           cxta_trendline_descriptor_period_arg(args, nargs, 0u, 2),
                           cxta_trendline_descriptor_period_arg(args, nargs, 1u, 2));
    if (out) memcpy(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_trendline_descriptor = {
    "trendline",
    2,
    2,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_trendline_output),
    0u,
    cxta_trendline_fields,
    CXTA_ARRAY_COUNT(cxta_trendline_fields),
    NULL,
    cxta_trendline_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_trendline_params,
    CXTA_ARRAY_COUNT(cxta_trendline_params),
    "price",
    &cxta_trendline_plot_descriptor,
};

static double cxta_trendline_project_at(size_t x1, double y1, size_t x2, double y2, size_t x) {
    if (x2 <= x1) return y2;
    return y2 + ((y2 - y1) / (double)(x2 - x1)) * (double)(x - x2);
}

static double cxta_trendline_slope_between(size_t x1, double y1, size_t x2, double y2) {
    if (x2 <= x1) return 0.0;
    return (y2 - y1) / (double)(x2 - x1);
}

static int cxta_trendline_is_pivot_high(const cxta_series_bar_view* view,
                                        size_t index,
                                        int left,
                                        int right) {
    const double pivot = view->bars[index].high;
    const size_t begin = index - (size_t)left;
    const size_t end = index + (size_t)right;

    for (size_t i = begin; i <= end; ++i) {
        if (i == index) continue;
        if (pivot <= view->bars[i].high) return 0;
    }
    return 1;
}

static int cxta_trendline_is_pivot_low(const cxta_series_bar_view* view,
                                       size_t index,
                                       int left,
                                       int right) {
    const double pivot = view->bars[index].low;
    const size_t begin = index - (size_t)left;
    const size_t end = index + (size_t)right;

    for (size_t i = begin; i <= end; ++i) {
        if (i == index) continue;
        if (pivot >= view->bars[i].low) return 0;
    }
    return 1;
}

cxta_trendline_output cxta_trendline(const cxta_series_bar_view* view,
                                     int left,
                                     int right) {
    cxta_trendline_output out = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    left = cxta_ts_clamp_period(left);
    right = cxta_ts_clamp_period(right);
    if (left < 1) left = 1;
    if (right < 1) right = 1;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        int has_high1 = 0;
        int has_high2 = 0;
        int has_low1 = 0;
        int has_low2 = 0;
        size_t hi1x = 0;
        size_t hi2x = 0;
        size_t lo1x = 0;
        size_t lo2x = 0;
        double hi1y = 0.0;
        double hi2y = 0.0;
        double lo1y = 0.0;
        double lo2y = 0.0;

        if (idx < (size_t)(left + right)) {
            out.upper = view->bars[idx].high;
            out.lower = view->bars[idx].low;
            out.active_index = (double)idx;
            return out;
        }

        for (size_t i = (size_t)left; i <= idx - (size_t)right; ++i) {
            if (cxta_trendline_is_pivot_high(view, i, left, right)) {
                has_high1 = has_high2;
                hi1x = hi2x;
                hi1y = hi2y;
                has_high2 = 1;
                hi2x = i;
                hi2y = view->bars[i].high;
            }
            if (cxta_trendline_is_pivot_low(view, i, left, right)) {
                has_low1 = has_low2;
                lo1x = lo2x;
                lo1y = lo2y;
                has_low2 = 1;
                lo2x = i;
                lo2y = view->bars[i].low;
            }
        }

        if (has_high1 && has_high2) {
            out.upper_slope = cxta_trendline_slope_between(hi1x, hi1y, hi2x, hi2y);
            out.upper = cxta_trendline_project_at(hi1x, hi1y, hi2x, hi2y, idx);
            out.upper_pivot = hi2y;
            out.upper_pivot_index = (double)hi2x;
        } else if (has_high2) {
            out.upper = hi2y;
            out.upper_pivot = hi2y;
            out.upper_pivot_index = (double)hi2x;
        } else {
            out.upper = view->bars[idx].high;
        }

        if (has_low1 && has_low2) {
            out.lower_slope = cxta_trendline_slope_between(lo1x, lo1y, lo2x, lo2y);
            out.lower = cxta_trendline_project_at(lo1x, lo1y, lo2x, lo2y, idx);
            out.lower_pivot = lo2y;
            out.lower_pivot_index = (double)lo2x;
        } else if (has_low2) {
            out.lower = lo2y;
            out.lower_pivot = lo2y;
            out.lower_pivot_index = (double)lo2x;
        } else {
            out.lower = view->bars[idx].low;
        }
        out.active_index = (double)idx;
    }
    return out;
}
