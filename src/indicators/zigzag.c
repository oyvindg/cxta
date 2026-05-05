/**
 * @file zigzag.c
 * @brief Causal ZigZag helpers.
 */

#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

#include <cxta/indicators/zigzag.h>

enum {
    CXTA_ZIGZAG_MAX_PIVOTS = 16,
    CXTA_ZIGZAG_MAX_ORDERED_PIVOTS = CXTA_ZIGZAG_MAX_PIVOTS * 2
};

static const cxta_field_descriptor cxta_zigzag_fields[] = {
    {"high", offsetof(cxta_zigzag_output, high), false},
    {"low", offsetof(cxta_zigzag_output, low), false},
    {"line", offsetof(cxta_zigzag_output, line), true},
    {"pivot_index", offsetof(cxta_zigzag_output, pivot_index), false},
    {"active", offsetof(cxta_zigzag_output, active), false},
    {"active_index", offsetof(cxta_zigzag_output, active_index), false},
    {"direction", offsetof(cxta_zigzag_output, direction), false},
    {"last", offsetof(cxta_zigzag_output, last), false},
    {"is_high", offsetof(cxta_zigzag_output, is_high), false},
};

static const cxta_plot_field_descriptor cxta_zigzag_plot_fields[] = {
    {"line", true, "ZigZag", "zigzag", "#f59e0b", "zigzag", NULL, NULL, NULL,
     "last", "pivot_index", "active", "active_index", true, true, NULL, NULL, true},
};

const cxta_indicator_plot_descriptor cxta_zigzag_plot_descriptor = {
    "zigzag",
    NULL,
    cxta_zigzag_plot_fields,
    CXTA_ARRAY_COUNT(cxta_zigzag_plot_fields),
};

typedef struct {
    double price;
    size_t index;
    int is_high;
} cxta_zigzag_pivot;

typedef enum {
    CXTA_ZIGZAG_PHASE_UNKNOWN = 0,
    CXTA_ZIGZAG_PHASE_UP = 1,
    CXTA_ZIGZAG_PHASE_DOWN = 2,
} cxta_zigzag_phase;

static double cxta_zigzag_descriptor_double_arg(const double* args,
                                                size_t nargs,
                                                size_t index,
                                                double fallback) {
    double raw;

    if (!args || index >= nargs) return fallback;
    raw = args[index];
    return isfinite(raw) ? raw : fallback;
}

static int cxta_zigzag_descriptor_int_arg(const double* args,
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

static int cxta_zigzag_descriptor_clamp_int_arg(const double* args,
                                                size_t nargs,
                                                size_t index,
                                                int fallback,
                                                int min_value,
                                                int max_value) {
    int value = cxta_zigzag_descriptor_int_arg(args, nargs, index, fallback);
    if (value < min_value) value = min_value;
    if (value > max_value) value = max_value;
    return value;
}

static void cxta_zigzag_descriptor_copy_struct(void* out,
                                               const void* value,
                                               size_t size) {
    if (!out || !value || size == 0u) return;
    memcpy(out, value, size);
}

static void cxta_zigzag_descriptor_eval(const cxta_series_bar_view* view,
                                        const double* args,
                                        size_t nargs,
                                        void* out) {
    const double threshold = cxta_zigzag_descriptor_double_arg(args, nargs, 0u, 0.03);
    const int pivot_offset =
        cxta_zigzag_descriptor_clamp_int_arg(args, nargs, 1u, 0, 0, INT_MAX);
    const cxta_zigzag_output value = cxta_zigzag(view, threshold, pivot_offset);
    cxta_zigzag_descriptor_copy_struct(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_zigzag_descriptor = {
    "zigzag",
    1,
    2,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT | CXTA_INDICATOR_REPAINTING,
    sizeof(cxta_zigzag_output),
    0u,
    cxta_zigzag_fields,
    CXTA_ARRAY_COUNT(cxta_zigzag_fields),
    NULL,
    cxta_zigzag_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_zigzag_params,
    CXTA_ARRAY_COUNT(cxta_zigzag_params),
    "zigzag",
    &cxta_zigzag_plot_descriptor,
};

static void cxta_zigzag_push_pivot(cxta_zigzag_pivot* pivots,
                                   int* count,
                                   double price,
                                   size_t index,
                                   int is_high) {
    if (*count > 0) {
        cxta_zigzag_pivot* prev = &pivots[*count - 1];
        if (prev->is_high == is_high) {
            const int replace = is_high ? (price >= prev->price) : (price <= prev->price);
            if (replace) {
                prev->price = price;
                prev->index = index;
                prev->is_high = is_high;
            }
            return;
        }
    }

    if (*count == CXTA_ZIGZAG_MAX_ORDERED_PIVOTS) {
        for (int i = 1; i < *count; ++i) pivots[i - 1] = pivots[i];
        --(*count);
    }

    pivots[*count].price = price;
    pivots[*count].index = index;
    pivots[*count].is_high = is_high;
    ++(*count);
}

cxta_zigzag_output cxta_zigzag(const cxta_series_bar_view* view,
                               double threshold,
                               int pivot_offset) {
    cxta_zigzag_output out = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    {
        const size_t len = cxta_series_clamp_index(view->size, view->index) + 1u;
        cxta_zigzag_pivot pivots[CXTA_ZIGZAG_MAX_ORDERED_PIVOTS];
        double highs[CXTA_ZIGZAG_MAX_PIVOTS];
        double lows[CXTA_ZIGZAG_MAX_PIVOTS];
        int pivot_count = 0;
        int high_count = 0;
        int low_count = 0;

        if (len < 2u || threshold <= 0.0) return out;
        if (pivot_offset < 0) pivot_offset = 0;

        {
            double high_extreme = view->bars[0].high;
            double low_extreme = view->bars[0].low;
            size_t high_index = 0u;
            size_t low_index = 0u;
            cxta_zigzag_phase phase = CXTA_ZIGZAG_PHASE_UNKNOWN;

            for (size_t i = 1u; i < len; ++i) {
                const double hi = view->bars[i].high;
                const double lo = view->bars[i].low;
                const double close = view->bars[i].close;

                if (phase == CXTA_ZIGZAG_PHASE_UNKNOWN) {
                    if (hi >= high_extreme) {
                        high_extreme = hi;
                        high_index = i;
                    }
                    if (lo <= low_extreme) {
                        low_extreme = lo;
                        low_index = i;
                    }

                    if (low_extreme > 0.0 &&
                        low_index < i &&
                        close >= low_extreme * (1.0 + threshold)) {
                        cxta_zigzag_push_pivot(pivots, &pivot_count, low_extreme, low_index, 0);
                        phase = CXTA_ZIGZAG_PHASE_UP;
                        high_extreme = hi;
                        high_index = i;
                        continue;
                    }

                    if (high_extreme > 0.0 &&
                        high_index < i &&
                        close <= high_extreme * (1.0 - threshold)) {
                        cxta_zigzag_push_pivot(pivots, &pivot_count, high_extreme, high_index, 1);
                        phase = CXTA_ZIGZAG_PHASE_DOWN;
                        low_extreme = lo;
                        low_index = i;
                        continue;
                    }
                } else if (phase == CXTA_ZIGZAG_PHASE_UP) {
                    if (hi >= high_extreme) {
                        high_extreme = hi;
                        high_index = i;
                    }
                    if (high_extreme > 0.0 &&
                        high_index < i &&
                        close <= high_extreme * (1.0 - threshold)) {
                        cxta_zigzag_push_pivot(pivots, &pivot_count, high_extreme, high_index, 1);
                        phase = CXTA_ZIGZAG_PHASE_DOWN;
                        low_extreme = lo;
                        low_index = i;
                    }
                } else {
                    if (lo <= low_extreme) {
                        low_extreme = lo;
                        low_index = i;
                    }
                    if (low_extreme > 0.0 &&
                        low_index < i &&
                        close >= low_extreme * (1.0 + threshold)) {
                        cxta_zigzag_push_pivot(pivots, &pivot_count, low_extreme, low_index, 0);
                        phase = CXTA_ZIGZAG_PHASE_UP;
                        high_extreme = hi;
                        high_index = i;
                    }
                }
            }

            if (pivot_offset == 0) {
                out.direction =
                    (phase == CXTA_ZIGZAG_PHASE_UP) ? 1.0 :
                    (phase == CXTA_ZIGZAG_PHASE_DOWN) ? -1.0 : 0.0;
                if (phase == CXTA_ZIGZAG_PHASE_UP) {
                    out.active = high_extreme;
                    out.active_index = (double)high_index;
                } else if (phase == CXTA_ZIGZAG_PHASE_DOWN) {
                    out.active = low_extreme;
                    out.active_index = (double)low_index;
                }
            }

            for (int i = pivot_count - 1; i >= 0; --i) {
                if (pivots[i].is_high) {
                    if (high_count < CXTA_ZIGZAG_MAX_PIVOTS) highs[high_count++] = pivots[i].price;
                } else {
                    if (low_count < CXTA_ZIGZAG_MAX_PIVOTS) lows[low_count++] = pivots[i].price;
                }
            }

            if (pivot_offset < high_count) out.high = highs[pivot_offset];
            if (pivot_offset < low_count) out.low = lows[pivot_offset];

            if (pivot_offset < pivot_count) {
                const cxta_zigzag_pivot pivot = pivots[pivot_count - 1 - pivot_offset];
                out.last = pivot.price;
                out.is_high = pivot.is_high ? 1.0 : 0.0;
                out.line = pivot.price;
                out.pivot_index = (double)pivot.index;
            }
        }
    }
    return out;
}
