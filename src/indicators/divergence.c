/**
 * @file divergence.c
 * @brief Two-series divergence helpers.
 */

#include <cxta/indicators/divergence.h>
#include <cxta/ts/smoothing.h>
#include <math.h>

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
