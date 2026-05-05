/**
 * @file divergence.c
 * @brief Two-series divergence helpers.
 */

#include <cxta/indicators/divergence.h>
#include <cxta/indicators/macros.h>
#include <cxta/ts/smoothing.h>
#include <math.h>
#include <stddef.h>

#define CXTA_DIV_FIELD(name, off) {(name), (off), true}

static const cxta_field_descriptor cxta_divergence_fields[] = {
    CXTA_DIV_FIELD("bullish", offsetof(cxta_divergence_output, bullish)),
    CXTA_DIV_FIELD("bearish", offsetof(cxta_divergence_output, bearish)),
    CXTA_DIV_FIELD("bull_segment", offsetof(cxta_divergence_output, bull_segment)),
    CXTA_DIV_FIELD("bear_segment", offsetof(cxta_divergence_output, bear_segment)),
};

static const cxta_plot_field_descriptor cxta_divergence_plot_fields[] = {
    CXTA_FIELD_PLOT("bullish", true, "Bullish Divergence", "divergence", "#22c55e", "hidden", "divergence", "Bullish divergence signal flag.", "Use transitions as reversal/continuation confirmation with price context."),
    CXTA_FIELD_PLOT("bearish", true, "Bearish Divergence", "divergence", "#ef4444", "hidden", "divergence", "Bearish divergence signal flag.", "Use transitions as reversal/continuation confirmation with price context."),
    CXTA_FIELD_PLOT("bull_segment", true, "Bull Segment", "divergence", "#22c55e", "line", "divergence", "Bullish divergence segment marker.", "Use to visualize the active bullish divergence span."),
    CXTA_FIELD_PLOT("bear_segment", true, "Bear Segment", "divergence", "#ef4444", "line", "divergence", "Bearish divergence segment marker.", "Use to visualize the active bearish divergence span."),
};

static const cxta_indicator_plot_descriptor cxta_divergence_plot_descriptor = {
    .indicator_name = "divergence",
    .fields = cxta_divergence_plot_fields,
    .field_count = CXTA_ARRAY_COUNT(cxta_divergence_plot_fields),
};

static const cxta_param_descriptor cxta_divergence_params[] = {
    {"source_a"},
    {"source_b"},
    {"left"},
    {"right"},
    {"lookback"},
};

static const cxta_expr_arg_descriptor cxta_divergence_expr_args[] = {
    {"source_a", CXTA_EXPR_ARG_SCALAR_SOURCE, NULL},
    {"source_b", CXTA_EXPR_ARG_SCALAR_SOURCE, NULL},
    {"left", CXTA_EXPR_ARG_NUMERIC, NULL},
    {"right", CXTA_EXPR_ARG_NUMERIC, NULL},
    {"lookback", CXTA_EXPR_ARG_NUMERIC, "200"},
};

const cxta_bridge_fn_spec cxta_divergence_bridge_fn_spec = CXTA_BRIDGE_FN_SPEC_EXPR(
    "divergence",
    4u,
    5u,
    cxta_divergence_params,
    cxta_divergence_expr_args,
    1);

const cxta_indicator_descriptor cxta_divergence_descriptor = {
    "divergence",
    2,
    3,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_divergence_output),
    0u,
    cxta_divergence_fields,
    CXTA_ARRAY_COUNT(cxta_divergence_fields),
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_divergence_params,
    CXTA_ARRAY_COUNT(cxta_divergence_params),
    "divergence",
    &cxta_divergence_plot_descriptor,
};

typedef struct {
    size_t index;
    double primary;
    double secondary;
    int valid;
} cxta_divergence_pivot;

static int cxta_divergence_is_pivot_low(const cxta_series_scalar_view* series,
                                        size_t index,
                                        int left,
                                        int right) {
    const double pivot = series->values[index];
    if (!isfinite(pivot)) return 0;

    for (size_t i = index - (size_t)left; i <= index + (size_t)right; ++i) {
        if (i == index) continue;
        if (pivot >= series->values[i]) return 0;
    }
    return 1;
}

static int cxta_divergence_is_pivot_high(const cxta_series_scalar_view* series,
                                         size_t index,
                                         int left,
                                         int right) {
    const double pivot = series->values[index];
    if (!isfinite(pivot)) return 0;

    for (size_t i = index - (size_t)left; i <= index + (size_t)right; ++i) {
        if (i == index) continue;
        if (pivot <= series->values[i]) return 0;
    }
    return 1;
}

static double cxta_divergence_segment_value(size_t start_index,
                                            double start_value,
                                            size_t end_index,
                                            double end_value,
                                            size_t target_index) {
    if (target_index < start_index || target_index > end_index) return NAN;
    if (end_index == start_index) return start_value;
    return start_value +
        ((end_value - start_value) / (double)(end_index - start_index)) *
        (double)(target_index - start_index);
}

cxta_divergence_output cxta_divergence_at(const cxta_series_scalar_view* primary,
                                          const cxta_series_scalar_view* secondary,
                                          int left,
                                          int right,
                                          int lookback,
                                          size_t query_index) {
    cxta_divergence_output out = {0.0, 0.0, NAN, NAN};
    if (!primary || !secondary) return out;
    if (!cxta_series_scalar_view_valid(primary) || !cxta_series_scalar_view_valid(secondary)) return out;

    left = cxta_ts_clamp_period(left);
    right = cxta_ts_clamp_period(right);
    lookback = cxta_ts_clamp_period(lookback);

    {
        const size_t primary_idx = cxta_series_clamp_index(primary->size, primary->index);
        const size_t secondary_idx = cxta_series_clamp_index(secondary->size, secondary->index);
        const size_t idx = (primary_idx < secondary_idx) ? primary_idx : secondary_idx;
        const size_t q = (query_index < idx) ? query_index : idx;
        cxta_divergence_pivot prev_low = {0u, 0.0, 0.0, 0};
        cxta_divergence_pivot prev_high = {0u, 0.0, 0.0, 0};

        if (idx < (size_t)(left + right)) return out;

        for (size_t confirm_index = (size_t)(left + right); confirm_index <= idx; ++confirm_index) {
            const size_t pivot_index = confirm_index - (size_t)right;

            if (pivot_index >= (size_t)left &&
                cxta_divergence_is_pivot_low(primary, pivot_index, left, right)) {
                cxta_divergence_pivot current = {
                    pivot_index,
                    primary->values[pivot_index],
                    secondary->values[pivot_index],
                    1
                };

                if (prev_low.valid &&
                    current.index > prev_low.index &&
                    (current.index - prev_low.index) <= (size_t)lookback &&
                    current.primary < prev_low.primary &&
                    current.secondary > prev_low.secondary) {
                    if (confirm_index == q) out.bullish = 1.0;
                    out.bull_segment = cxta_divergence_segment_value(
                        prev_low.index,
                        prev_low.secondary,
                        current.index,
                        current.secondary,
                        q);
                }

                prev_low = current;
            }

            if (pivot_index >= (size_t)left &&
                cxta_divergence_is_pivot_high(primary, pivot_index, left, right)) {
                cxta_divergence_pivot current = {
                    pivot_index,
                    primary->values[pivot_index],
                    secondary->values[pivot_index],
                    1
                };

                if (prev_high.valid &&
                    current.index > prev_high.index &&
                    (current.index - prev_high.index) <= (size_t)lookback &&
                    current.primary > prev_high.primary &&
                    current.secondary < prev_high.secondary) {
                    if (confirm_index == q) out.bearish = 1.0;
                    out.bear_segment = cxta_divergence_segment_value(
                        prev_high.index,
                        prev_high.secondary,
                        current.index,
                        current.secondary,
                        q);
                }

                prev_high = current;
            }
        }
    }
    return out;
}

cxta_divergence_output cxta_divergence(const cxta_series_scalar_view* primary,
                                       const cxta_series_scalar_view* secondary,
                                       int left,
                                       int right,
                                       int lookback) {
    if (!primary || !secondary) {
        const cxta_divergence_output out = {0.0, 0.0, NAN, NAN};
        return out;
    }

    {
        const size_t primary_idx = cxta_series_clamp_index(primary->size, primary->index);
        const size_t secondary_idx = cxta_series_clamp_index(secondary->size, secondary->index);
        const size_t query_index = (primary_idx < secondary_idx) ? primary_idx : secondary_idx;
        return cxta_divergence_at(primary, secondary, left, right, lookback, query_index);
    }
}
