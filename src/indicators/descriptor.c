/**
 * @file descriptor.c
 * @brief Descriptor metadata for expression-facing indicators.
 */

#include <cxta/cxta.h>

#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define CXTA_DESCRIPTOR_SCALAR_EX(name, min_args, max_args, state_sz, eval_scalar_fn)      \
    {                                                                                       \
        (name), (min_args), (max_args), -1, -1, -1,                                        \
        CXTA_INDICATOR_SCALAR, 0u, (state_sz), NULL, 0u,                                   \
        (eval_scalar_fn), NULL, NULL, NULL, NULL, NULL,                                    \
        NULL, 0u                                                                            \
    }

#define CXTA_DESCRIPTOR_SCALAR_EX_PARAMS(name, min_args, max_args, state_sz, eval_scalar_fn, params_array) \
    {                                                                                                       \
        (name), (min_args), (max_args), -1, -1, -1,                                                        \
        CXTA_INDICATOR_SCALAR, 0u, (state_sz), NULL, 0u,                                                   \
        (eval_scalar_fn), NULL, NULL, NULL, NULL, NULL,                                                    \
        (params_array), CXTA_ARRAY_COUNT(params_array)                                                     \
    }

#define CXTA_DESCRIPTOR_SCALAR_FIELD_EX(name, min_args, max_args, state_sz, fields_array, eval_scalar_fn) \
    {                                                                                                      \
        (name), (min_args), (max_args), -1, -1, -1,                                                       \
        CXTA_INDICATOR_SCALAR, 0u, (state_sz), (fields_array), CXTA_ARRAY_COUNT(fields_array),            \
        (eval_scalar_fn), NULL, NULL, NULL, NULL, NULL,                                                   \
        NULL, 0u                                                                                           \
    }

#define CXTA_DESCRIPTOR_SCALAR_SOURCE_EX(name, min_args, max_args, ss_min, ss_max, state_sz, eval_scalar_fn, eval_scalar_src_fn) \
    {                                                                                                                            \
        (name), (min_args), (max_args), (ss_min), (ss_max), -1,                                                                \
        CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_SCALAR_SOURCE, 0u, (state_sz), NULL, 0u,                                       \
        (eval_scalar_fn), NULL, (eval_scalar_src_fn), NULL, NULL, NULL,                                                        \
        NULL, 0u                                                                                                                \
    }

#define CXTA_DESCRIPTOR_SCALAR_SOURCE_STEP_EX(name, min_args, max_args, ss_min, ss_max, state_sz, eval_scalar_fn, eval_scalar_src_fn, step_scalar_fn) \
    {                                                                                                                                               \
        (name), (min_args), (max_args), (ss_min), (ss_max), -1,                                                                                   \
        CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_SCALAR_SOURCE, 0u, (state_sz), NULL, 0u,                                                          \
        (eval_scalar_fn), NULL, (eval_scalar_src_fn), NULL, (step_scalar_fn), NULL,                                                               \
        NULL, 0u                                                                                                                                   \
    }

#define CXTA_DESCRIPTOR_SCALAR_SOURCE_STEP_EX_PARAMS(name, min_args, max_args, ss_min, ss_max, state_sz, eval_scalar_fn, eval_scalar_src_fn, step_scalar_fn, params_array) \
    {                                                                                                                                                                      \
        (name), (min_args), (max_args), (ss_min), (ss_max), -1,                                                                                                          \
        CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_SCALAR_SOURCE, 0u, (state_sz), NULL, 0u,                                                                                  \
        (eval_scalar_fn), NULL, (eval_scalar_src_fn), NULL, (step_scalar_fn), NULL,                                                                                       \
        (params_array), CXTA_ARRAY_COUNT(params_array)                                                                                                                    \
    }

#define CXTA_DESCRIPTOR_SCALAR_SOURCE_DYNAMIC_STEP_EX(name, min_args, max_args, ss_min, ss_max, state_slots_fn, eval_scalar_fn, eval_scalar_src_fn, step_scalar_fn) \
    {                                                                                                                                                           \
        (name), (min_args), (max_args), (ss_min), (ss_max), -1,                                                                                               \
        CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_SCALAR_SOURCE, 0u, 0u, NULL, 0u,                                                                              \
        (eval_scalar_fn), NULL, (eval_scalar_src_fn), (state_slots_fn), (step_scalar_fn), NULL,                                                               \
        NULL, 0u                                                                                                                                               \
    }

#define CXTA_DESCRIPTOR_STRUCT_EX(name, min_args, max_args, primary_index, type, state_sz, fields_array, eval_struct_fn) \
    {                                                                                                                     \
        (name), (min_args), (max_args), -1, -1, (primary_index),                                                         \
        CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT, sizeof(type), (state_sz),                                         \
        (fields_array), CXTA_ARRAY_COUNT(fields_array),                                                                   \
        NULL, (eval_struct_fn), NULL, NULL, NULL, NULL,                                                                   \
        NULL, 0u                                                                                                           \
    }

#define CXTA_DESCRIPTOR_STRUCT_EX_PARAMS(name, min_args, max_args, primary_index, type, state_sz, fields_array, eval_struct_fn, params_array) \
    {                                                                                                                                          \
        (name), (min_args), (max_args), -1, -1, (primary_index),                                                                              \
        CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT, sizeof(type), (state_sz),                                                              \
        (fields_array), CXTA_ARRAY_COUNT(fields_array),                                                                                        \
        NULL, (eval_struct_fn), NULL, NULL, NULL, NULL,                                                                                        \
        (params_array), CXTA_ARRAY_COUNT(params_array)                                                                                         \
    }

#define CXTA_DESCRIPTOR_STRUCT_STEP_EX(name, min_args, max_args, primary_index, type, state_sz, fields_array, eval_struct_fn, step_struct_fn) \
    {                                                                                                                                           \
        (name), (min_args), (max_args), -1, -1, (primary_index),                                                                               \
        CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT, sizeof(type), (state_sz),                                                               \
        (fields_array), CXTA_ARRAY_COUNT(fields_array),                                                                                         \
        NULL, (eval_struct_fn), NULL, NULL, NULL, (step_struct_fn),                                                                             \
        NULL, 0u                                                                                                                                 \
    }

#define CXTA_FIELD(name, offset) {(name), (offset), true}
#define CXTA_FIELD_HIDDEN(name, offset) {(name), (offset), false}

static int cxta_descriptor_int_arg(const double* args,
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

static int cxta_descriptor_period_arg(const double* args,
                                      size_t nargs,
                                      size_t index,
                                      int fallback) {
    return cxta_ts_clamp_period(cxta_descriptor_int_arg(args, nargs, index, fallback));
}

static int cxta_descriptor_clamp_int_arg(const double* args,
                                         size_t nargs,
                                         size_t index,
                                         int fallback,
                                         int min_value,
                                         int max_value) {
    int value = cxta_descriptor_int_arg(args, nargs, index, fallback);
    if (value < min_value) value = min_value;
    if (value > max_value) value = max_value;
    return value;
}

static double cxta_descriptor_double_arg(const double* args,
                                         size_t nargs,
                                         size_t index,
                                         double fallback) {
    double raw;

    if (!args || index >= nargs) return fallback;
    raw = args[index];
    return isfinite(raw) ? raw : fallback;
}

static int cxta_descriptor_parse_pivot_args(const cxta_series_bar_view* view,
                                            const double* args,
                                            size_t nargs,
                                            cxta_struct_pivot_args* out) {
    const size_t current_index =
        (view && view->size > 0u) ? cxta_series_clamp_index(view->size, view->index) : 0u;
    return cxta_struct_pivot_args_parse(args, nargs, current_index, out);
}

static void cxta_descriptor_copy_struct(void* out, const void* value, size_t size) {
    if (!out || !value || size == 0u) return;
    memcpy(out, value, size);
}

#define CXTA_WRAP_BAR_SCALAR_0(wrapper, fn)                                                   \
    static double wrapper(const cxta_series_bar_view* view, const double* args, size_t nargs) { \
        (void)args;                                                                           \
        (void)nargs;                                                                          \
        return fn(view);                                                                      \
    }

#define CXTA_WRAP_BAR_SCALAR_1I(wrapper, fn, default0)                                        \
    static double wrapper(const cxta_series_bar_view* view, const double* args, size_t nargs) { \
        return fn(view, cxta_descriptor_period_arg(args, nargs, 0u, (default0)));            \
    }

#define CXTA_WRAP_BAR_SCALAR_2I(wrapper, fn, default0, default1)                              \
    static double wrapper(const cxta_series_bar_view* view, const double* args, size_t nargs) { \
        return fn(view,                                                                       \
                  cxta_descriptor_period_arg(args, nargs, 0u, (default0)),                    \
                  cxta_descriptor_period_arg(args, nargs, 1u, (default1)));                   \
    }

#define CXTA_WRAP_BAR_SCALAR_3I(wrapper, fn, default0, default1, default2)                    \
    static double wrapper(const cxta_series_bar_view* view, const double* args, size_t nargs) { \
        return fn(view,                                                                       \
                  cxta_descriptor_period_arg(args, nargs, 0u, (default0)),                    \
                  cxta_descriptor_period_arg(args, nargs, 1u, (default1)),                    \
                  cxta_descriptor_period_arg(args, nargs, 2u, (default2)));                   \
    }

#define CXTA_WRAP_BAR_SCALAR_4I(wrapper, fn, default0, default1, default2, default3)          \
    static double wrapper(const cxta_series_bar_view* view, const double* args, size_t nargs) { \
        return fn(view,                                                                       \
                  cxta_descriptor_period_arg(args, nargs, 0u, (default0)),                    \
                  cxta_descriptor_period_arg(args, nargs, 1u, (default1)),                    \
                  cxta_descriptor_period_arg(args, nargs, 2u, (default2)),                    \
                  cxta_descriptor_period_arg(args, nargs, 3u, (default3)));                   \
    }

#define CXTA_WRAP_BAR_STRUCT_1I(wrapper, type, fn, default0)                                  \
    static void wrapper(const cxta_series_bar_view* view,                                     \
                        const double* args,                                                   \
                        size_t nargs,                                                         \
                        void* out) {                                                          \
        type value;                                                                           \
        memset(&value, 0, sizeof(value));                                                     \
        value = fn(view, cxta_descriptor_period_arg(args, nargs, 0u, (default0)));           \
        cxta_descriptor_copy_struct(out, &value, sizeof(value));                              \
    }

#define CXTA_WRAP_BAR_STRUCT_2I(wrapper, type, fn, default0, default1)                        \
    static void wrapper(const cxta_series_bar_view* view,                                     \
                        const double* args,                                                   \
                        size_t nargs,                                                         \
                        void* out) {                                                          \
        type value;                                                                           \
        memset(&value, 0, sizeof(value));                                                     \
        value = fn(view,                                                                      \
                   cxta_descriptor_period_arg(args, nargs, 0u, (default0)),                  \
                   cxta_descriptor_period_arg(args, nargs, 1u, (default1)));                 \
        cxta_descriptor_copy_struct(out, &value, sizeof(value));                              \
    }

#define CXTA_WRAP_BAR_STRUCT_3I(wrapper, type, fn, default0, default1, default2)              \
    static void wrapper(const cxta_series_bar_view* view,                                     \
                        const double* args,                                                   \
                        size_t nargs,                                                         \
                        void* out) {                                                          \
        type value;                                                                           \
        memset(&value, 0, sizeof(value));                                                     \
        value = fn(view,                                                                      \
                   cxta_descriptor_period_arg(args, nargs, 0u, (default0)),                  \
                   cxta_descriptor_period_arg(args, nargs, 1u, (default1)),                  \
                   cxta_descriptor_period_arg(args, nargs, 2u, (default2)));                 \
        cxta_descriptor_copy_struct(out, &value, sizeof(value));                              \
    }

#define CXTA_WRAP_BAR_STRUCT_1I_1D(wrapper, type, fn, default0, default1)                     \
    static void wrapper(const cxta_series_bar_view* view,                                     \
                        const double* args,                                                   \
                        size_t nargs,                                                         \
                        void* out) {                                                          \
        type value;                                                                           \
        memset(&value, 0, sizeof(value));                                                     \
        value = fn(view,                                                                      \
                   cxta_descriptor_period_arg(args, nargs, 0u, (default0)),                  \
                   cxta_descriptor_double_arg(args, nargs, 1u, (default1)));                 \
        cxta_descriptor_copy_struct(out, &value, sizeof(value));                              \
    }

#define CXTA_WRAP_BAR_STRUCT_2D(wrapper, type, fn, default0, default1)                        \
    static void wrapper(const cxta_series_bar_view* view,                                     \
                        const double* args,                                                   \
                        size_t nargs,                                                         \
                        void* out) {                                                          \
        type value;                                                                           \
        memset(&value, 0, sizeof(value));                                                     \
        value = fn(view,                                                                      \
                   cxta_descriptor_double_arg(args, nargs, 0u, (default0)),                  \
                   cxta_descriptor_double_arg(args, nargs, 1u, (default1)));                 \
        cxta_descriptor_copy_struct(out, &value, sizeof(value));                              \
    }

static double cxta_desc_eval_scalar_window_extrema(const cxta_series_scalar_view* source,
                                                   const double* args,
                                                   size_t nargs,
                                                   int want_max) {
    size_t idx;
    size_t start;
    size_t window;
    size_t i;
    double value;

    if (!source || !cxta_series_scalar_view_valid(source)) return 0.0;
    idx = cxta_series_clamp_index(source->size, source->index);
    window = (size_t)cxta_descriptor_period_arg(args, nargs, 0u, 1);
    if (window > idx + 1u) window = idx + 1u;
    start = idx + 1u - window;
    value = source->values[start];

    for (i = start + 1u; i <= idx; ++i) {
        if (want_max) {
            if (source->values[i] > value) value = source->values[i];
        } else {
            if (source->values[i] < value) value = source->values[i];
        }
    }

    return value;
}

static double cxta_desc_eval_bar_window_extrema(const cxta_series_bar_view* view,
                                                const double* args,
                                                size_t nargs,
                                                size_t field_offset,
                                                int want_max) {
    size_t idx;
    size_t start;
    size_t window;
    size_t i;
    double value;
    const cxta_series_bar* bar;

    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;
    idx = cxta_series_clamp_index(view->size, view->index);
    window = (size_t)cxta_descriptor_period_arg(args, nargs, 0u, 1);
    if (window > idx + 1u) window = idx + 1u;
    start = idx + 1u - window;
    bar = &view->bars[start];
    value = *(const double*)((const unsigned char*)bar + field_offset);

    for (i = start + 1u; i <= idx; ++i) {
        double sample;
        bar = &view->bars[i];
        sample = *(const double*)((const unsigned char*)bar + field_offset);
        if (want_max) {
            if (sample > value) value = sample;
        } else {
            if (sample < value) value = sample;
        }
    }

    return value;
}

static double cxta_desc_eval_rolling_max(const cxta_series_bar_view* view,
                                         const double* args,
                                         size_t nargs) {
    return cxta_desc_eval_bar_window_extrema(
        view, args, nargs, offsetof(cxta_series_bar, high), 1);
}

static double cxta_desc_eval_rolling_min(const cxta_series_bar_view* view,
                                         const double* args,
                                         size_t nargs) {
    return cxta_desc_eval_bar_window_extrema(
        view, args, nargs, offsetof(cxta_series_bar, low), 0);
}

static double cxta_desc_eval_rolling_max_close(const cxta_series_bar_view* view,
                                               const double* args,
                                               size_t nargs) {
    return cxta_desc_eval_bar_window_extrema(
        view, args, nargs, offsetof(cxta_series_bar, close), 1);
}

static double cxta_desc_eval_rolling_min_close(const cxta_series_bar_view* view,
                                               const double* args,
                                               size_t nargs) {
    return cxta_desc_eval_bar_window_extrema(
        view, args, nargs, offsetof(cxta_series_bar, close), 0);
}

static double cxta_desc_eval_rolling_max_scalar_src(const cxta_series_scalar_view* source,
                                                    const double* args,
                                                    size_t nargs) {
    return cxta_desc_eval_scalar_window_extrema(source, args, nargs, 1);
}

static double cxta_desc_eval_rolling_min_scalar_src(const cxta_series_scalar_view* source,
                                                    const double* args,
                                                    size_t nargs) {
    return cxta_desc_eval_scalar_window_extrema(source, args, nargs, 0);
}

static double cxta_desc_midpoint_sma(const cxta_series_bar_view* view,
                                     size_t idx,
                                     int period) {
    const size_t window = ((size_t)period < (idx + 1u)) ? (size_t)period : (idx + 1u);
    const size_t start = idx + 1u - window;
    double sum = 0.0;
    size_t i;

    for (i = start; i <= idx; ++i) {
        sum += (view->bars[i].high + view->bars[i].low) * 0.5;
    }
    return cxta_sma_from_sum(sum, window);
}

CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_volume_sma, cxta_volume_sma, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_dema, cxta_dema, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_tema, cxta_tema, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_hma, cxta_hma, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_linreg_slope, cxta_linreg_slope, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_linreg_angle, cxta_linreg_angle, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_rvol, cxta_rvol, 20)
CXTA_WRAP_BAR_SCALAR_0(cxta_desc_eval_truerange, cxta_truerange)
CXTA_WRAP_BAR_SCALAR_0(cxta_desc_eval_typical, cxta_typical)
CXTA_WRAP_BAR_SCALAR_0(cxta_desc_eval_median_price, cxta_median_price)
CXTA_WRAP_BAR_SCALAR_0(cxta_desc_eval_weighted_close, cxta_wclose)
CXTA_WRAP_BAR_SCALAR_0(cxta_desc_eval_obv, cxta_obv)

static double cxta_desc_eval_awesome_oscillator(const cxta_series_bar_view* view,
                                                const double* args,
                                                size_t nargs) {
    size_t idx;
    int fast;
    int slow;
    int swap_tmp;

    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;
    idx = cxta_series_clamp_index(view->size, view->index);
    fast = cxta_descriptor_period_arg(args, nargs, 0u, 5);
    slow = cxta_descriptor_period_arg(args, nargs, 1u, 34);
    if (fast > slow) {
        swap_tmp = fast;
        fast = slow;
        slow = swap_tmp;
    }
    return cxta_desc_midpoint_sma(view, idx, fast)
        - cxta_desc_midpoint_sma(view, idx, slow);
}

CXTA_WRAP_BAR_SCALAR_0(cxta_desc_eval_bop, cxta_bop)

static double cxta_desc_eval_ease_of_movement(const cxta_series_bar_view* view,
                                              const double* args,
                                              size_t nargs) {
    size_t idx;
    size_t window;
    size_t start;
    size_t i;
    double sum = 0.0;

    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;
    idx = cxta_series_clamp_index(view->size, view->index);
    if (idx == 0u) return 0.0;

    window = (size_t)cxta_descriptor_period_arg(args, nargs, 0u, 14);
    if (window > idx) window = idx;
    start = idx - window + 1u;
    for (i = start; i <= idx; ++i) {
        const cxta_series_bar* cur = &view->bars[i];
        const cxta_series_bar* prev = &view->bars[i - 1u];
        const double mid_move = ((cur->high + cur->low) * 0.5)
                              - ((prev->high + prev->low) * 0.5);
        const double box_ratio = (cur->volume <= 1e-12)
                               ? 0.0
                               : ((cur->high - cur->low) / cur->volume);
        sum += mid_move * box_ratio;
    }
    return sum / (double)window;
}

CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_volume_ema, cxta_volume_ema, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_williams_r, cxta_williamsr, 14)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_cci, cxta_cci, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_mfi, cxta_mfi, 14)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_cmf, cxta_cmf, 20)
CXTA_WRAP_BAR_SCALAR_2I(cxta_desc_eval_chaikin_oscillator, cxta_chaikin, 3, 10)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_dpo, cxta_dpo, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_cmo, cxta_cmo, 14)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_trix, cxta_trix, 15)
CXTA_WRAP_BAR_SCALAR_2I(cxta_desc_eval_tsi, cxta_tsi, 25, 13)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_kama, cxta_kama, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_frama, cxta_frama, 16)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_vidya, cxta_vidya, 14)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_fisher, cxta_fisher, 10)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_choppiness_index, cxta_choppiness, 14)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_ulcer_index, cxta_ulcer, 14)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_rvi, cxta_rvi, 10)

static double cxta_desc_eval_historical_volatility(const cxta_series_bar_view* view,
                                                   const double* args,
                                                   size_t nargs) {
    const int period = cxta_descriptor_period_arg(args, nargs, 0u, 20);
    const int annualization = cxta_descriptor_clamp_int_arg(args, nargs, 1u, 252, 1, INT_MAX);
    const double base = cxta_hvol(view, period);
    if (annualization == 252) return base;
    return base * sqrt((double)annualization / 252.0);
}

static double cxta_desc_eval_elder_force(const cxta_series_bar_view* view,
                                         const double* args,
                                         size_t nargs) {
    return cxta_efi(view, cxta_descriptor_period_arg(args, nargs, 0u, 1));
}

static double cxta_desc_eval_ppo(const cxta_series_bar_view* view,
                                 const double* args,
                                 size_t nargs) {
    int fast = cxta_descriptor_period_arg(args, nargs, 0u, 12);
    int slow = cxta_descriptor_period_arg(args, nargs, 1u, 26);
    int swap_tmp;

    if (fast > slow) {
        swap_tmp = fast;
        fast = slow;
        slow = swap_tmp;
    }
    return cxta_ppo(view, fast, slow);
}

static double cxta_desc_eval_kst(const cxta_series_bar_view* view,
                                 const double* args,
                                 size_t nargs) {
    return cxta_kst(view,
                    cxta_descriptor_period_arg(args, nargs, 0u, 10),
                    cxta_descriptor_period_arg(args, nargs, 1u, 15),
                    cxta_descriptor_period_arg(args, nargs, 2u, 20),
                    cxta_descriptor_period_arg(args, nargs, 3u, 30));
}

static double cxta_desc_eval_mass_index(const cxta_series_bar_view* view,
                                        const double* args,
                                        size_t nargs) {
    return cxta_mass_index(view,
                           cxta_descriptor_period_arg(args, nargs, 0u, 25),
                           cxta_descriptor_period_arg(args, nargs, 1u, 9));
}

static double cxta_desc_eval_nvi(const cxta_series_bar_view* view,
                                 const double* args,
                                 size_t nargs) {
    (void)args;
    (void)nargs;
    return cxta_nvi(view);
}

static double cxta_desc_eval_pvi(const cxta_series_bar_view* view,
                                 const double* args,
                                 size_t nargs) {
    (void)args;
    (void)nargs;
    return cxta_pvi(view);
}

static double cxta_desc_eval_connors_rsi(const cxta_series_bar_view* view,
                                         const double* args,
                                         size_t nargs) {
    return cxta_connors_rsi(view,
                            cxta_descriptor_period_arg(args, nargs, 0u, 3),
                            cxta_descriptor_period_arg(args, nargs, 1u, 2),
                            cxta_descriptor_period_arg(args, nargs, 2u, 100));
}

static double cxta_desc_eval_coppock_curve(const cxta_series_bar_view* view,
                                           const double* args,
                                           size_t nargs) {
    return cxta_coppock(view,
                        cxta_descriptor_period_arg(args, nargs, 0u, 10),
                        cxta_descriptor_period_arg(args, nargs, 1u, 14),
                        cxta_descriptor_period_arg(args, nargs, 2u, 11));
}

static double cxta_desc_eval_schaff_trend_cycle(const cxta_series_bar_view* view,
                                                const double* args,
                                                size_t nargs) {
    return cxta_schaff(view,
                       cxta_descriptor_period_arg(args, nargs, 0u, 23),
                       cxta_descriptor_period_arg(args, nargs, 1u, 50),
                       cxta_descriptor_period_arg(args, nargs, 2u, 10));
}

static double cxta_desc_eval_vwap(const cxta_series_bar_view* view,
                                  const double* args,
                                  size_t nargs) {
    int period = cxta_descriptor_int_arg(args, nargs, 0u, 0);
    if (period > 0) period = cxta_ts_clamp_period(period);
    return cxta_vwap(view, period);
}

static double cxta_desc_eval_dominant_cycle_period(const cxta_series_bar_view* view,
                                                   const double* args,
                                                   size_t nargs) {
    int min_period = cxta_descriptor_period_arg(args, nargs, 0u, 10);
    int max_period = cxta_descriptor_period_arg(args, nargs, 1u, 40);
    if (min_period < 5) min_period = 5;
    if (max_period < min_period + 2) max_period = min_period + 2;
    return cxta_dominant_cycle(view, min_period, max_period);
}

static double cxta_desc_eval_anchored_vwap(const cxta_series_bar_view* view,
                                           const double* args,
                                           size_t nargs) {
    int anchor_bars = cxta_descriptor_int_arg(args, nargs, 0u, 0);
    if (anchor_bars > 0) anchor_bars = cxta_ts_clamp_period(anchor_bars);
    return cxta_anchored_vwap(view, anchor_bars);
}

CXTA_WRAP_BAR_STRUCT_1I(cxta_desc_eval_adx, cxta_adx_output, cxta_adx, 14)

static void cxta_desc_eval_supertrend(const cxta_series_bar_view* view,
                                      const double* args,
                                      size_t nargs,
                                      void* out) {
    const int period = cxta_descriptor_period_arg(args, nargs, 0u, 10);
    const double multiplier = cxta_descriptor_double_arg(args, nargs, 1u, 3.0);
    const cxta_supertrend_output value = cxta_supertrend(view, period, multiplier);
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

CXTA_WRAP_BAR_STRUCT_1I(cxta_desc_eval_donchian, cxta_channel_output, cxta_donchian, 20)

static void cxta_desc_eval_keltner(const cxta_series_bar_view* view,
                                   const double* args,
                                   size_t nargs,
                                   void* out) {
    const int ema_period = cxta_descriptor_period_arg(args, nargs, 0u, 20);
    const double atr_mult = cxta_descriptor_double_arg(args, nargs, 1u, 2.0);
    const int atr_period = cxta_descriptor_period_arg(args, nargs, 2u, ema_period);
    const cxta_channel_output value = cxta_keltner(view, ema_period, atr_mult, atr_period);
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

CXTA_WRAP_BAR_STRUCT_1I(cxta_desc_eval_price_channel, cxta_channel_output, cxta_price_channel, 20)
CXTA_WRAP_BAR_STRUCT_1I(cxta_desc_eval_aroon, cxta_aroon_output, cxta_aroon, 14)

static void cxta_desc_eval_pvo(const cxta_series_bar_view* view,
                               const double* args,
                               size_t nargs,
                               void* out) {
    int fast = cxta_descriptor_period_arg(args, nargs, 0u, 12);
    int slow = cxta_descriptor_period_arg(args, nargs, 1u, 26);
    int signal = cxta_descriptor_period_arg(args, nargs, 2u, 9);
    int swap_tmp;
    cxta_pvo_output value;

    if (fast > slow) {
        swap_tmp = fast;
        fast = slow;
        slow = swap_tmp;
    }
    value = cxta_pvo(view, fast, slow, signal);
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_desc_eval_stochastic(const cxta_series_bar_view* view,
                                      const double* args,
                                      size_t nargs,
                                      void* out) {
    const cxta_stoch_output value =
        cxta_stochastic(view,
                        cxta_descriptor_period_arg(args, nargs, 0u, 14),
                        cxta_descriptor_period_arg(args, nargs, 1u, 3),
                        cxta_descriptor_period_arg(args, nargs, 2u, 3));
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_desc_eval_stoch_rsi(const cxta_series_bar_view* view,
                                     const double* args,
                                     size_t nargs,
                                     void* out) {
    int rsi_period = cxta_descriptor_period_arg(args, nargs, 0u, 14);
    int stoch_period = cxta_descriptor_period_arg(args, nargs, 1u, rsi_period);
    int smooth_k = cxta_descriptor_period_arg(args, nargs, 2u, 3);
    int smooth_d = cxta_descriptor_period_arg(args, nargs, 3u, 3);
    cxta_stochrsi_output value = cxta_stochrsi(view, rsi_period, stoch_period, smooth_k, smooth_d);
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_desc_eval_mama(const cxta_series_bar_view* view,
                                const double* args,
                                size_t nargs,
                                void* out) {
    double fast_limit = cxta_descriptor_double_arg(args, nargs, 0u, 0.5);
    double slow_limit = cxta_descriptor_double_arg(args, nargs, 1u, 0.05);
    cxta_mama_output value;

    if (fast_limit < 0.01) fast_limit = 0.01;
    if (fast_limit > 0.99) fast_limit = 0.99;
    if (slow_limit < 0.001) slow_limit = 0.001;
    if (slow_limit > fast_limit) slow_limit = fast_limit;
    value = cxta_mama(view, fast_limit, slow_limit);
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

CXTA_WRAP_BAR_STRUCT_1I(cxta_desc_eval_vortex, cxta_vortex_output, cxta_vortex, 14)

static void cxta_desc_eval_ichimoku(const cxta_series_bar_view* view,
                                    const double* args,
                                    size_t nargs,
                                    void* out) {
    int tenkan = cxta_descriptor_period_arg(args, nargs, 0u, 9);
    int kijun = cxta_descriptor_period_arg(args, nargs, 1u, 26);
    int senkou_b = cxta_descriptor_period_arg(args, nargs, 2u, 52);
    int swap_tmp;
    cxta_ichimoku_output value;

    if (tenkan > kijun) {
        swap_tmp = tenkan;
        tenkan = kijun;
        kijun = swap_tmp;
    }
    if (senkou_b < kijun + 1) senkou_b = kijun + 1;
    value = cxta_ichimoku(view, tenkan, kijun, senkou_b);
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

CXTA_WRAP_BAR_STRUCT_2D(cxta_desc_eval_parabolic_sar, cxta_psar_output, cxta_psar, 0.02, 0.2)

static void cxta_desc_eval_ttm_squeeze(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs,
                                       void* out) {
    const cxta_squeeze_output value =
        cxta_ttm_squeeze(view,
                         cxta_descriptor_period_arg(args, nargs, 0u, 20),
                         cxta_descriptor_double_arg(args, nargs, 1u, 2.0),
                         cxta_descriptor_period_arg(args, nargs, 2u, 20),
                         cxta_descriptor_double_arg(args, nargs, 3u, 1.5),
                         cxta_descriptor_period_arg(args, nargs, 4u, 20));
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

CXTA_WRAP_BAR_STRUCT_2I(cxta_desc_eval_trendline, cxta_trendline_output, cxta_trendline, 2, 2)

static void cxta_desc_eval_volume_profile(const cxta_series_bar_view* view,
                                          const double* args,
                                          size_t nargs,
                                          void* out) {
    const int period = cxta_descriptor_period_arg(args, nargs, 0u, 20);
    const int bins = cxta_descriptor_clamp_int_arg(args, nargs, 1u, 24, 5, 200);
    const cxta_vp_output value = cxta_volume_profile(view, period, bins);
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_desc_eval_swing_pivots(const cxta_series_bar_view* view,
                                        const double* args,
                                        size_t nargs,
                                        void* out) {
    cxta_struct_pivot_state value = {0};
    cxta_struct_pivot_args parsed = {0};

    if (view && cxta_descriptor_parse_pivot_args(view, args, nargs, &parsed)) {
        (void)cxta_struct_pivot_state_compute(view, &parsed, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_desc_eval_structure(const cxta_series_bar_view* view,
                                     const double* args,
                                     size_t nargs,
                                     void* out) {
    cxta_struct_pivot_state pivot = {0};
    cxta_struct_structure_state value = {0};
    cxta_struct_pivot_args parsed = {0};

    if (view && cxta_descriptor_parse_pivot_args(view, args, nargs, &parsed)) {
        (void)cxta_struct_pivot_state_compute(view, &parsed, &pivot);
        (void)cxta_struct_structure_state_compute(&pivot, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_desc_eval_bos(const cxta_series_bar_view* view,
                               const double* args,
                               size_t nargs,
                               void* out) {
    cxta_struct_pivot_state pivot = {0};
    cxta_struct_bos_state value = {0};
    cxta_struct_pivot_args parsed = {0};

    if (view && cxta_descriptor_parse_pivot_args(view, args, nargs, &parsed)) {
        (void)cxta_struct_pivot_state_compute(view, &parsed, &pivot);
        (void)cxta_struct_bos_state_compute(view, &pivot, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_desc_eval_fvg(const cxta_series_bar_view* view,
                               const double* args,
                               size_t nargs,
                               void* out) {
    cxta_struct_fvg_state value = {0};

    if (view && cxta_series_bar_view_valid(view)) {
        const int fallback = (int)cxta_series_clamp_index(view->size, view->index) + 1;
        const size_t lookback =
            (size_t)cxta_descriptor_period_arg(args, nargs, 0u, fallback);
        (void)cxta_struct_fvg_state_compute(view, lookback, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_desc_eval_order_block(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs,
                                       void* out) {
    cxta_struct_order_block_state value = {0};
    cxta_struct_pivot_args parsed = {0};

    if (view && cxta_descriptor_parse_pivot_args(view, args, nargs, &parsed)) {
        (void)cxta_struct_order_block_state_compute(view, &parsed, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_desc_eval_liquidity(const cxta_series_bar_view* view,
                                     const double* args,
                                     size_t nargs,
                                     void* out) {
    cxta_struct_pivot_state pivot = {0};
    cxta_struct_liquidity_state value = {0};
    cxta_struct_pivot_args parsed = {0};
    const double tolerance = cxta_descriptor_double_arg(args, nargs, 3u, 0.001);

    if (view && cxta_descriptor_parse_pivot_args(view, args, nargs, &parsed)) {
        (void)cxta_struct_pivot_state_compute(view, &parsed, &pivot);
        (void)cxta_struct_liquidity_state_compute(view, &pivot, tolerance, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_desc_eval_sfp(const cxta_series_bar_view* view,
                               const double* args,
                               size_t nargs,
                               void* out) {
    cxta_struct_pivot_state pivot = {0};
    cxta_struct_sfp_state value = {0};
    cxta_struct_pivot_args parsed = {0};

    if (view && cxta_descriptor_parse_pivot_args(view, args, nargs, &parsed)) {
        (void)cxta_struct_pivot_state_compute(view, &parsed, &pivot);
        (void)cxta_struct_sfp_state_compute(view, &pivot, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_desc_eval_swing_anchor_vwap(const cxta_series_bar_view* view,
                                             const double* args,
                                             size_t nargs,
                                             void* out) {
    cxta_struct_vwap_result value = {0};
    cxta_struct_vwap_args parsed = {0};

    if (view && cxta_struct_vwap_args_parse(args, nargs, &parsed)) {
        (void)cxta_struct_vwap_compute(view, &parsed, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_desc_step_swing_anchor_vwap(const cxta_series_bar_view* view,
                                             size_t index,
                                             const double* args,
                                             size_t nargs,
                                             void* state,
                                             void* out) {
    cxta_struct_vwap_result value = {0};
    cxta_struct_vwap_args parsed = {0};

    if (view && state && cxta_struct_vwap_args_parse(args, nargs, &parsed)) {
        (void)cxta_struct_vwap_step(
            view, index, &parsed, (cxta_struct_vwap_state*)state, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_desc_eval_wedge(const cxta_series_bar_view* view,
                                 const double* args,
                                 size_t nargs,
                                 void* out) {
    cxta_struct_wedge_state value = {0};
    cxta_struct_wedge_args parsed = {0};
    const size_t current_index =
        (view && view->size > 0u) ? cxta_series_clamp_index(view->size, view->index) : 0u;

    if (view && cxta_struct_wedge_args_parse(args, nargs, current_index, &parsed)) {
        (void)cxta_struct_wedge_state_compute(view, &parsed, &value);
        cxta_struct_wedge_state_mask_geometry(&value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_desc_eval_pivot_points(const cxta_series_bar_view* view,
                                        const double* args,
                                        size_t nargs,
                                        void* out) {
    cxta_struct_pivot_points_result value = {0};
    (void)args;
    (void)nargs;

    if (view) {
        (void)cxta_struct_pivot_points_compute(view, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static const cxta_field_descriptor kVwapFields[] = {
    CXTA_FIELD("value", CXTA_FIELD_OFFSET_SCALAR),
};

static const cxta_field_descriptor kAdxFields[] = {
    CXTA_FIELD("adx", offsetof(cxta_adx_output, adx)),
    CXTA_FIELD("plusDI", offsetof(cxta_adx_output, plus_di)),
    CXTA_FIELD("minusDI", offsetof(cxta_adx_output, minus_di)),
};

static const cxta_field_descriptor kSupertrendFields[] = {
    CXTA_FIELD("value", offsetof(cxta_supertrend_output, value)),
    CXTA_FIELD("direction", offsetof(cxta_supertrend_output, direction)),
    CXTA_FIELD("strength", offsetof(cxta_supertrend_output, strength)),
};

static const cxta_field_descriptor kChannelFields[] = {
    CXTA_FIELD("upper", offsetof(cxta_channel_output, upper)),
    CXTA_FIELD("lower", offsetof(cxta_channel_output, lower)),
    CXTA_FIELD("middle", offsetof(cxta_channel_output, middle)),
    CXTA_FIELD("width", offsetof(cxta_channel_output, width)),
};

static const cxta_field_descriptor kAroonFields[] = {
    CXTA_FIELD("up", offsetof(cxta_aroon_output, up)),
    CXTA_FIELD("down", offsetof(cxta_aroon_output, down)),
    CXTA_FIELD("oscillator", offsetof(cxta_aroon_output, oscillator)),
};

static const cxta_field_descriptor kPvoFields[] = {
    CXTA_FIELD("line", offsetof(cxta_pvo_output, line)),
    CXTA_FIELD("signal", offsetof(cxta_pvo_output, signal)),
    CXTA_FIELD("histogram", offsetof(cxta_pvo_output, histogram)),
};

static const cxta_field_descriptor kStochasticFields[] = {
    CXTA_FIELD("k", offsetof(cxta_stoch_output, k)),
    CXTA_FIELD("d", offsetof(cxta_stoch_output, d)),
};

static const cxta_field_descriptor kStochRsiFields[] = {
    CXTA_FIELD("k", offsetof(cxta_stochrsi_output, k)),
    CXTA_FIELD("d", offsetof(cxta_stochrsi_output, d)),
};

static const cxta_field_descriptor kMamaFields[] = {
    CXTA_FIELD("mama", offsetof(cxta_mama_output, mama)),
    CXTA_FIELD("fama", offsetof(cxta_mama_output, fama)),
};

static const cxta_field_descriptor kVortexFields[] = {
    CXTA_FIELD("plusVI", offsetof(cxta_vortex_output, vi_plus)),
    CXTA_FIELD("minusVI", offsetof(cxta_vortex_output, vi_minus)),
};

static const cxta_field_descriptor kIchimokuFields[] = {
    CXTA_FIELD("tenkan", offsetof(cxta_ichimoku_output, tenkan)),
    CXTA_FIELD("kijun", offsetof(cxta_ichimoku_output, kijun)),
    CXTA_FIELD("senkouA", offsetof(cxta_ichimoku_output, senkou_a)),
    CXTA_FIELD("senkouB", offsetof(cxta_ichimoku_output, senkou_b)),
    CXTA_FIELD("chikou", offsetof(cxta_ichimoku_output, chikou)),
};

static const cxta_field_descriptor kPsarFields[] = {
    CXTA_FIELD("value", offsetof(cxta_psar_output, value)),
    CXTA_FIELD("direction", offsetof(cxta_psar_output, direction)),
};

static const cxta_field_descriptor kTtmSqueezeFields[] = {
    CXTA_FIELD("squeeze_on", offsetof(cxta_squeeze_output, squeeze_on)),
    CXTA_FIELD("squeeze_off", offsetof(cxta_squeeze_output, squeeze_off)),
    CXTA_FIELD("momentum", offsetof(cxta_squeeze_output, momentum)),
};

static const cxta_field_descriptor kTrendlineFields[] = {
    CXTA_FIELD("upper", offsetof(cxta_trendline_output, upper)),
    CXTA_FIELD("lower", offsetof(cxta_trendline_output, lower)),
    CXTA_FIELD("upperSlope", offsetof(cxta_trendline_output, upper_slope)),
    CXTA_FIELD("lowerSlope", offsetof(cxta_trendline_output, lower_slope)),
};

static const cxta_field_descriptor kVolumeProfileFields[] = {
    CXTA_FIELD("poc", offsetof(cxta_vp_output, poc)),
    CXTA_FIELD("vah", offsetof(cxta_vp_output, vah)),
    CXTA_FIELD("val", offsetof(cxta_vp_output, val)),
    CXTA_FIELD("range_high", offsetof(cxta_vp_output, range_high)),
    CXTA_FIELD("range_low", offsetof(cxta_vp_output, range_low)),
};

static const cxta_field_descriptor kSwingPivotsFields[] = {
    CXTA_FIELD("high", offsetof(cxta_struct_pivot_state, high)),
    CXTA_FIELD("low", offsetof(cxta_struct_pivot_state, low)),
    CXTA_FIELD("is_high", offsetof(cxta_struct_pivot_state, is_high)),
    CXTA_FIELD("is_low", offsetof(cxta_struct_pivot_state, is_low)),
};

static const cxta_field_descriptor kPivotPointsFields[] = {
    CXTA_FIELD("pp", offsetof(cxta_struct_pivot_points_result, pp)),
    CXTA_FIELD("r1", offsetof(cxta_struct_pivot_points_result, r1)),
    CXTA_FIELD("r2", offsetof(cxta_struct_pivot_points_result, r2)),
    CXTA_FIELD("r3", offsetof(cxta_struct_pivot_points_result, r3)),
    CXTA_FIELD("s1", offsetof(cxta_struct_pivot_points_result, s1)),
    CXTA_FIELD("s2", offsetof(cxta_struct_pivot_points_result, s2)),
    CXTA_FIELD("s3", offsetof(cxta_struct_pivot_points_result, s3)),
};

static const cxta_field_descriptor kStructureFields[] = {
    CXTA_FIELD("hh", offsetof(cxta_struct_structure_state, hh)),
    CXTA_FIELD("hl", offsetof(cxta_struct_structure_state, hl)),
    CXTA_FIELD("lh", offsetof(cxta_struct_structure_state, lh)),
    CXTA_FIELD("ll", offsetof(cxta_struct_structure_state, ll)),
    CXTA_FIELD("last_high", offsetof(cxta_struct_structure_state, last_high)),
    CXTA_FIELD("last_low", offsetof(cxta_struct_structure_state, last_low)),
    CXTA_FIELD("strength", offsetof(cxta_struct_structure_state, strength)),
};

static const cxta_field_descriptor kBosFields[] = {
    CXTA_FIELD("bos_up", offsetof(cxta_struct_bos_state, bos_up)),
    CXTA_FIELD("bos_down", offsetof(cxta_struct_bos_state, bos_down)),
    CXTA_FIELD("level", offsetof(cxta_struct_bos_state, level)),
    CXTA_FIELD("choch_up", offsetof(cxta_struct_bos_state, choch_up)),
    CXTA_FIELD("choch_down", offsetof(cxta_struct_bos_state, choch_down)),
};

static const cxta_field_descriptor kFvgFields[] = {
    CXTA_FIELD_HIDDEN("bullish", offsetof(cxta_struct_fvg_state, bullish)),
    CXTA_FIELD_HIDDEN("bearish", offsetof(cxta_struct_fvg_state, bearish)),
    CXTA_FIELD("top", offsetof(cxta_struct_fvg_state, top)),
    CXTA_FIELD("bottom", offsetof(cxta_struct_fvg_state, bottom)),
    CXTA_FIELD("filled", offsetof(cxta_struct_fvg_state, filled)),
};

static const cxta_field_descriptor kOrderBlockFields[] = {
    CXTA_FIELD("bullish", offsetof(cxta_struct_order_block_state, bullish)),
    CXTA_FIELD("bearish", offsetof(cxta_struct_order_block_state, bearish)),
    CXTA_FIELD("top", offsetof(cxta_struct_order_block_state, top)),
    CXTA_FIELD("bottom", offsetof(cxta_struct_order_block_state, bottom)),
    CXTA_FIELD("strength", offsetof(cxta_struct_order_block_state, strength)),
};

static const cxta_field_descriptor kLiquidityFields[] = {
    CXTA_FIELD_HIDDEN("buy_side", offsetof(cxta_struct_liquidity_state, buy_side)),
    CXTA_FIELD_HIDDEN("sell_side", offsetof(cxta_struct_liquidity_state, sell_side)),
    CXTA_FIELD("level", offsetof(cxta_struct_liquidity_state, level)),
    CXTA_FIELD("swept", offsetof(cxta_struct_liquidity_state, swept)),
};

static const cxta_field_descriptor kSfpFields[] = {
    CXTA_FIELD("sfp_high", offsetof(cxta_struct_sfp_state, sfp_high)),
    CXTA_FIELD("sfp_low", offsetof(cxta_struct_sfp_state, sfp_low)),
    CXTA_FIELD("level", offsetof(cxta_struct_sfp_state, level)),
};

static const cxta_field_descriptor kSwingAnchorVwapFields[] = {
    CXTA_FIELD("value", offsetof(cxta_struct_vwap_result, value)),
    CXTA_FIELD("direction", offsetof(cxta_struct_vwap_result, direction)),
    CXTA_FIELD("reanchor", offsetof(cxta_struct_vwap_result, reanchor)),
    CXTA_FIELD("anchor_price", offsetof(cxta_struct_vwap_result, anchor_price)),
    CXTA_FIELD("anchor_bars", offsetof(cxta_struct_vwap_result, anchor_bars)),
    CXTA_FIELD("hh", offsetof(cxta_struct_vwap_result, hh)),
    CXTA_FIELD("hl", offsetof(cxta_struct_vwap_result, hl)),
    CXTA_FIELD("lh", offsetof(cxta_struct_vwap_result, lh)),
    CXTA_FIELD("ll", offsetof(cxta_struct_vwap_result, ll)),
    CXTA_FIELD("last_high", offsetof(cxta_struct_vwap_result, last_high)),
    CXTA_FIELD("last_low", offsetof(cxta_struct_vwap_result, last_low)),
    CXTA_FIELD("bull_vwap", offsetof(cxta_struct_vwap_result, bull_vwap)),
    CXTA_FIELD("bear_vwap", offsetof(cxta_struct_vwap_result, bear_vwap)),
};

static const cxta_field_descriptor kDivergenceFields[] = {
    CXTA_FIELD("bullish", offsetof(cxta_divergence_output, bullish)),
    CXTA_FIELD("bearish", offsetof(cxta_divergence_output, bearish)),
    CXTA_FIELD("bull_segment", offsetof(cxta_divergence_output, bull_segment)),
    CXTA_FIELD("bear_segment", offsetof(cxta_divergence_output, bear_segment)),
};

static const cxta_param_descriptor kDivergenceBridgeParams[] = {
    {"source_a"},
    {"source_b"},
    {"left"},
    {"right"},
    {"lookback"},
};

static const cxta_expr_arg_descriptor kDivergenceExprArgs[] = {
    {"source_a", CXTA_EXPR_ARG_SCALAR_SOURCE, NULL},
    {"source_b", CXTA_EXPR_ARG_SCALAR_SOURCE, NULL},
    {"left", CXTA_EXPR_ARG_NUMERIC, NULL},
    {"right", CXTA_EXPR_ARG_NUMERIC, NULL},
    {"lookback", CXTA_EXPR_ARG_NUMERIC, "200"},
};

static const cxta_bridge_fn_spec kDivergenceBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC_EXPR(
        "divergence",
        4u,
        5u,
        kDivergenceBridgeParams,
        kDivergenceExprArgs,
        1);

static const cxta_param_descriptor kCrossPairParams[] = {
    {"a"},
    {"b"},
};

static const cxta_param_descriptor kCrossPairPeriodParams[] = {
    {"a"},
    {"b"},
    {"period"},
};

static const cxta_bridge_fn_spec kSpreadBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("spread", 2u, 2u, kCrossPairParams, 0);
static const cxta_bridge_fn_spec kPairSpreadBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("pair_spread", 2u, 2u, kCrossPairParams, 0);
static const cxta_bridge_fn_spec kCovarianceBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("covariance", 3u, 3u, kCrossPairPeriodParams, 0);
static const cxta_bridge_fn_spec kRollingCorrBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("rolling_corr", 3u, 3u, kCrossPairPeriodParams, 0);
static const cxta_bridge_fn_spec kRollingBetaBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("rolling_beta", 3u, 3u, kCrossPairPeriodParams, 0);
static const cxta_bridge_fn_spec kRollingAlphaBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("rolling_alpha", 3u, 3u, kCrossPairPeriodParams, 0);
static const cxta_bridge_fn_spec kHvRatioBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("hv_ratio", 3u, 3u, kCrossPairPeriodParams, 0);
static const cxta_bridge_fn_spec kTrackingErrorBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("tracking_error", 3u, 3u, kCrossPairPeriodParams, 0);
static const cxta_bridge_fn_spec kRelativeStrengthBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("relative_strength", 3u, 3u, kCrossPairPeriodParams, 0);
static const cxta_bridge_fn_spec kZScoreBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("z_score", 3u, 3u, kCrossPairPeriodParams, 0);

static const cxta_field_descriptor kWedgeFields[] = {
    CXTA_FIELD("upper", offsetof(cxta_struct_wedge_state, upper)),
    CXTA_FIELD("lower", offsetof(cxta_struct_wedge_state, lower)),
    CXTA_FIELD("width", offsetof(cxta_struct_wedge_state, width)),
    CXTA_FIELD("upperSlope", offsetof(cxta_struct_wedge_state, upper_slope)),
    CXTA_FIELD("lowerSlope", offsetof(cxta_struct_wedge_state, lower_slope)),
    CXTA_FIELD_HIDDEN("active", offsetof(cxta_struct_wedge_state, active)),
    CXTA_FIELD_HIDDEN("rising", offsetof(cxta_struct_wedge_state, rising)),
    CXTA_FIELD_HIDDEN("falling", offsetof(cxta_struct_wedge_state, falling)),
    CXTA_FIELD_HIDDEN("breakdown", offsetof(cxta_struct_wedge_state, breakdown)),
    CXTA_FIELD_HIDDEN("breakout", offsetof(cxta_struct_wedge_state, breakout)),
};

static const cxta_indicator_descriptor kCoreDescriptors[] = {
    CXTA_DESCRIPTOR_SCALAR_SOURCE_EX("rolling_max", 1, 1, 1, 1, 0u, cxta_desc_eval_rolling_max, cxta_desc_eval_rolling_max_scalar_src),
    CXTA_DESCRIPTOR_SCALAR_SOURCE_EX("rolling_min", 1, 1, 1, 1, 0u, cxta_desc_eval_rolling_min, cxta_desc_eval_rolling_min_scalar_src),
    CXTA_DESCRIPTOR_SCALAR_SOURCE_EX("rolling_max_close", 1, 1, 1, 1, 0u, cxta_desc_eval_rolling_max_close, cxta_desc_eval_rolling_max_scalar_src),
    CXTA_DESCRIPTOR_SCALAR_SOURCE_EX("rolling_min_close", 1, 1, 1, 1, 0u, cxta_desc_eval_rolling_min_close, cxta_desc_eval_rolling_min_scalar_src),
    CXTA_DESCRIPTOR_STRUCT_EX("adx", 1, 1, 0, cxta_adx_output, sizeof(cxta_adx_state), kAdxFields, cxta_desc_eval_adx),
    CXTA_DESCRIPTOR_STRUCT_EX("supertrend", 2, 2, 0, cxta_supertrend_output, sizeof(cxta_supertrend_state), kSupertrendFields, cxta_desc_eval_supertrend),
    CXTA_DESCRIPTOR_STRUCT_EX("donchian", 1, 1, 2, cxta_channel_output, 0u, kChannelFields, cxta_desc_eval_donchian),
    CXTA_DESCRIPTOR_STRUCT_EX("keltner", 2, 3, 2, cxta_channel_output, 0u, kChannelFields, cxta_desc_eval_keltner),
    CXTA_DESCRIPTOR_STRUCT_EX("price_channel", 1, 1, 2, cxta_channel_output, 0u, kChannelFields, cxta_desc_eval_price_channel),
    CXTA_DESCRIPTOR_SCALAR_EX("volume_sma", 1, 1, 0u, cxta_desc_eval_volume_sma),
    CXTA_DESCRIPTOR_SCALAR_EX("dema", 1, 1, sizeof(cxta_dema_state), cxta_desc_eval_dema),
    CXTA_DESCRIPTOR_SCALAR_EX("tema", 1, 1, sizeof(cxta_tema_state), cxta_desc_eval_tema),
    CXTA_DESCRIPTOR_SCALAR_EX("hma", 1, 1, 0u, cxta_desc_eval_hma),
    CXTA_DESCRIPTOR_SCALAR_EX("linear_regression_slope", 1, 1, 0u, cxta_desc_eval_linreg_slope),
    CXTA_DESCRIPTOR_SCALAR_EX("linreg_angle", 1, 1, 0u, cxta_desc_eval_linreg_angle),
    CXTA_DESCRIPTOR_SCALAR_EX("historical_volatility", 1, 2, 0u, cxta_desc_eval_historical_volatility),
    CXTA_DESCRIPTOR_SCALAR_EX("realized_volatility", 1, 1, 0u, cxta_desc_eval_rvol),
    CXTA_DESCRIPTOR_SCALAR_EX("true_range", 0, 0, 0u, cxta_desc_eval_truerange),
    CXTA_DESCRIPTOR_SCALAR_EX("typical_price", 0, 0, 0u, cxta_desc_eval_typical),
    CXTA_DESCRIPTOR_SCALAR_EX("median_price", 0, 0, 0u, cxta_desc_eval_median_price),
    CXTA_DESCRIPTOR_SCALAR_EX("weighted_close", 0, 0, 0u, cxta_desc_eval_weighted_close),
    CXTA_DESCRIPTOR_SCALAR_EX("obv", 0, 0, sizeof(cxta_obv_state), cxta_desc_eval_obv),
    CXTA_DESCRIPTOR_SCALAR_EX("awesome_oscillator", 2, 2, 0u, cxta_desc_eval_awesome_oscillator),
    CXTA_DESCRIPTOR_SCALAR_EX("bop", 0, 0, 0u, cxta_desc_eval_bop),
    CXTA_DESCRIPTOR_SCALAR_EX("ease_of_movement", 1, 1, 0u, cxta_desc_eval_ease_of_movement),
    CXTA_DESCRIPTOR_SCALAR_EX("volume_ema", 1, 1, sizeof(cxta_volume_ema_state), cxta_desc_eval_volume_ema),
    CXTA_DESCRIPTOR_STRUCT_EX("aroon", 1, 1, 2, cxta_aroon_output, 0u, kAroonFields, cxta_desc_eval_aroon),
    CXTA_DESCRIPTOR_SCALAR_EX("williams_r", 1, 1, 0u, cxta_desc_eval_williams_r),
    CXTA_DESCRIPTOR_SCALAR_EX("cci", 1, 1, 0u, cxta_desc_eval_cci),
    CXTA_DESCRIPTOR_SCALAR_EX("mfi", 1, 1, 0u, cxta_desc_eval_mfi),
    CXTA_DESCRIPTOR_SCALAR_EX("cmf", 1, 1, 0u, cxta_desc_eval_cmf),
    CXTA_DESCRIPTOR_SCALAR_EX("chaikin_oscillator", 2, 2, sizeof(cxta_chaikin_state), cxta_desc_eval_chaikin_oscillator),
    CXTA_DESCRIPTOR_SCALAR_EX("dpo", 1, 1, 0u, cxta_desc_eval_dpo),
    CXTA_DESCRIPTOR_SCALAR_EX("cmo", 1, 1, 0u, cxta_desc_eval_cmo),
    CXTA_DESCRIPTOR_SCALAR_EX("ppo", 2, 2, sizeof(cxta_ppo_state), cxta_desc_eval_ppo),
    CXTA_DESCRIPTOR_STRUCT_EX("pvo", 2, 3, 0, cxta_pvo_output, sizeof(cxta_pvo_state), kPvoFields, cxta_desc_eval_pvo),
    CXTA_DESCRIPTOR_SCALAR_EX("trix", 1, 1, sizeof(cxta_trix_state), cxta_desc_eval_trix),
    CXTA_DESCRIPTOR_SCALAR_EX("tsi", 2, 2, sizeof(cxta_tsi_state), cxta_desc_eval_tsi),
    CXTA_DESCRIPTOR_SCALAR_EX("kst", 0, 4, 0u, cxta_desc_eval_kst),
    CXTA_DESCRIPTOR_SCALAR_EX("mass_index", 1, 2, 0u, cxta_desc_eval_mass_index),
    CXTA_DESCRIPTOR_SCALAR_EX("elder_force", 0, 1, sizeof(cxta_efi_state), cxta_desc_eval_elder_force),
    CXTA_DESCRIPTOR_SCALAR_EX("nvi", 0, 0, sizeof(cxta_nvi_state), cxta_desc_eval_nvi),
    CXTA_DESCRIPTOR_SCALAR_EX("pvi", 0, 0, sizeof(cxta_nvi_state), cxta_desc_eval_pvi),
    CXTA_DESCRIPTOR_STRUCT_EX("stochastic", 1, 3, 0, cxta_stoch_output, 0u, kStochasticFields, cxta_desc_eval_stochastic),
    CXTA_DESCRIPTOR_STRUCT_EX("stoch_rsi", 1, 4, 0, cxta_stochrsi_output, 0u, kStochRsiFields, cxta_desc_eval_stoch_rsi),
    CXTA_DESCRIPTOR_STRUCT_EX("mama", 0, 2, 0, cxta_mama_output, 0u, kMamaFields, cxta_desc_eval_mama),
    CXTA_DESCRIPTOR_SCALAR_EX("kama", 1, 1, 0u, cxta_desc_eval_kama),
    CXTA_DESCRIPTOR_SCALAR_EX("frama", 1, 1, 0u, cxta_desc_eval_frama),
    CXTA_DESCRIPTOR_SCALAR_EX("vidya", 1, 1, 0u, cxta_desc_eval_vidya),
    CXTA_DESCRIPTOR_SCALAR_EX("fisher_transform", 1, 1, 0u, cxta_desc_eval_fisher),
    CXTA_DESCRIPTOR_SCALAR_EX("crsi", 0, 3, 0u, cxta_desc_eval_connors_rsi),
    CXTA_DESCRIPTOR_SCALAR_EX("coppock_curve", 0, 3, 0u, cxta_desc_eval_coppock_curve),
    CXTA_DESCRIPTOR_SCALAR_EX("schaff_trend_cycle", 3, 3, 0u, cxta_desc_eval_schaff_trend_cycle),
    CXTA_DESCRIPTOR_SCALAR_EX("choppiness_index", 1, 1, 0u, cxta_desc_eval_choppiness_index),
    CXTA_DESCRIPTOR_SCALAR_EX("ulcer_index", 1, 1, 0u, cxta_desc_eval_ulcer_index),
    CXTA_DESCRIPTOR_SCALAR_EX("rvi", 1, 1, 0u, cxta_desc_eval_rvi),
    CXTA_DESCRIPTOR_SCALAR_FIELD_EX("vwap", 0, 1, sizeof(cxta_vwap_state), kVwapFields, cxta_desc_eval_vwap),
    CXTA_DESCRIPTOR_STRUCT_EX("vortex", 1, 1, 0, cxta_vortex_output, 0u, kVortexFields, cxta_desc_eval_vortex),
    CXTA_DESCRIPTOR_SCALAR_EX("dominant_cycle_period", 0, 2, 0u, cxta_desc_eval_dominant_cycle_period),
    CXTA_DESCRIPTOR_STRUCT_EX("ichimoku", 3, 3, 1, cxta_ichimoku_output, 0u, kIchimokuFields, cxta_desc_eval_ichimoku),
    CXTA_DESCRIPTOR_STRUCT_EX("parabolic_sar", 0, 2, 0, cxta_psar_output, sizeof(cxta_psar_state), kPsarFields, cxta_desc_eval_parabolic_sar),
    CXTA_DESCRIPTOR_STRUCT_EX("ttm_squeeze", 0, 5, 2, cxta_squeeze_output, 0u, kTtmSqueezeFields, cxta_desc_eval_ttm_squeeze),
    CXTA_DESCRIPTOR_STRUCT_EX("trendline", 2, 2, 0, cxta_trendline_output, 0u, kTrendlineFields, cxta_desc_eval_trendline),
    CXTA_DESCRIPTOR_STRUCT_EX("volume_profile", 1, 2, 0, cxta_vp_output, 0u, kVolumeProfileFields, cxta_desc_eval_volume_profile),
    CXTA_DESCRIPTOR_STRUCT_EX("swing_pivots", 2, 3, 0, cxta_struct_pivot_state, 0u, kSwingPivotsFields, cxta_desc_eval_swing_pivots),
    CXTA_DESCRIPTOR_STRUCT_EX("pivot_points", 0, 0, 0, cxta_struct_pivot_points_result, 0u, kPivotPointsFields, cxta_desc_eval_pivot_points),
    CXTA_DESCRIPTOR_STRUCT_EX("structure", 2, 3, 6, cxta_struct_structure_state, 0u, kStructureFields, cxta_desc_eval_structure),
    CXTA_DESCRIPTOR_STRUCT_EX("bos", 2, 3, 0, cxta_struct_bos_state, 0u, kBosFields, cxta_desc_eval_bos),
    CXTA_DESCRIPTOR_STRUCT_EX("fvg", 0, 1, 0, cxta_struct_fvg_state, 0u, kFvgFields, cxta_desc_eval_fvg),
    CXTA_DESCRIPTOR_STRUCT_EX("order_block", 2, 3, 0, cxta_struct_order_block_state, 0u, kOrderBlockFields, cxta_desc_eval_order_block),
    CXTA_DESCRIPTOR_STRUCT_EX("liquidity", 2, 4, 0, cxta_struct_liquidity_state, 0u, kLiquidityFields, cxta_desc_eval_liquidity),
    CXTA_DESCRIPTOR_STRUCT_EX("sfp", 2, 3, 0, cxta_struct_sfp_state, 0u, kSfpFields, cxta_desc_eval_sfp),
    CXTA_DESCRIPTOR_STRUCT_STEP_EX("swing_anchor_vwap", 0, 7, 0, cxta_struct_vwap_result, sizeof(cxta_struct_vwap_state), kSwingAnchorVwapFields, cxta_desc_eval_swing_anchor_vwap, cxta_desc_step_swing_anchor_vwap),
    CXTA_DESCRIPTOR_STRUCT_EX("wedge", 2, 4, 5, cxta_struct_wedge_state, 0u, kWedgeFields, cxta_desc_eval_wedge),
    CXTA_DESCRIPTOR_STRUCT_EX("divergence", 2, 3, 0, cxta_divergence_output, 0u, kDivergenceFields, NULL),
    CXTA_DESCRIPTOR_SCALAR_EX("anchored_vwap", 0, 1, 0u, cxta_desc_eval_anchored_vwap),
};

static const cxta_indicator_descriptor* const kExternalDescriptors[] = {
    &cxta_macd_descriptor,
    &cxta_rsi_descriptor,
    &cxta_ema_descriptor,
    &cxta_atr_descriptor,
    &cxta_sma_descriptor,
    &cxta_rma_descriptor,
    &cxta_wma_descriptor,
    &cxta_stddev_descriptor,
    &cxta_zscore_descriptor,
    &cxta_roc_descriptor,
    &cxta_bollinger_descriptor,
    &cxta_zigzag_descriptor,
    &cxta_fair_value_gap_descriptor,
};

static const cxta_bridge_fn_spec* const kBridgeFnSpecs[] = {
    &cxta_macd_bridge_fn_spec,
    &cxta_rsi_bridge_fn_spec,
    &cxta_ema_bridge_fn_spec,
    &cxta_atr_bridge_fn_spec,
    &cxta_sma_bridge_fn_spec,
    &cxta_rma_bridge_fn_spec,
    &cxta_wma_bridge_fn_spec,
    &cxta_stddev_bridge_fn_spec,
    &cxta_zscore_bridge_fn_spec,
    &cxta_roc_bridge_fn_spec,
    &cxta_bollinger_bridge_fn_spec,
    &cxta_zigzag_bridge_fn_spec,
    &cxta_fair_value_gap_bridge_fn_spec,
    &kDivergenceBridgeFnSpec,
    &kSpreadBridgeFnSpec,
    &kPairSpreadBridgeFnSpec,
    &kCovarianceBridgeFnSpec,
    &kRollingCorrBridgeFnSpec,
    &kRollingBetaBridgeFnSpec,
    &kRollingAlphaBridgeFnSpec,
    &kHvRatioBridgeFnSpec,
    &kTrackingErrorBridgeFnSpec,
    &kRelativeStrengthBridgeFnSpec,
    &kZScoreBridgeFnSpec,
};

const cxta_indicator_descriptor* cxta_indicator_descriptors(size_t* count) {
    static cxta_indicator_descriptor descriptors[
        CXTA_ARRAY_COUNT(kCoreDescriptors) + CXTA_ARRAY_COUNT(kExternalDescriptors)];
    static int initialized = 0;
    size_t out_index;
    size_t i;

    if (!initialized) {
        memcpy(descriptors, kCoreDescriptors, sizeof(kCoreDescriptors));
        out_index = CXTA_ARRAY_COUNT(kCoreDescriptors);
        for (i = 0u; i < CXTA_ARRAY_COUNT(kExternalDescriptors); ++i) {
            descriptors[out_index++] = *kExternalDescriptors[i];
        }
        initialized = 1;
    }
    if (count) {
        *count = CXTA_ARRAY_COUNT(kCoreDescriptors) + CXTA_ARRAY_COUNT(kExternalDescriptors);
    }
    return descriptors;
}

const cxta_bridge_fn_spec* const* cxta_bridge_fn_specs(size_t* count) {
    if (count) *count = CXTA_ARRAY_COUNT(kBridgeFnSpecs);
    return kBridgeFnSpecs;
}

const cxta_bridge_fn_spec* cxta_bridge_fn_spec_find(const char* name) {
    size_t count = 0u;
    const cxta_bridge_fn_spec* const* specs = cxta_bridge_fn_specs(&count);
    size_t i;

    if (!name) return NULL;
    for (i = 0u; i < count; ++i) {
        const cxta_bridge_fn_spec* spec = specs[i];
        if (spec && spec->name && strcmp(spec->name, name) == 0) return spec;
    }
    return NULL;
}

const cxta_indicator_descriptor* cxta_indicator_descriptor_find(const char* name) {
    size_t count = 0;
    const cxta_indicator_descriptor* descriptors = cxta_indicator_descriptors(&count);
    size_t i;

    if (!name) return NULL;

    for (i = 0; i < count; ++i) {
        if (strcmp(descriptors[i].name, name) == 0) return &descriptors[i];
    }
    return NULL;
}

/* -------------------------------------------------------------------------- */
/* Named parameter specs                                                       */
/* -------------------------------------------------------------------------- */

static const char* const kParamPeriod[]    = {"period"};
static const char* const kParamLookback[]  = {"lookback"};
static const char* const kParamBand[]      = {"period", "mult"};
static const char* const kParamHistVol[]   = {"period", "annualization"};
static const char* const kParamVolProf[]   = {"period", "bins"};
static const char* const kParamMama[]      = {"fast_limit", "slow_limit"};
static const char* const kParamDomCycle[]  = {"min_period", "max_period"};
static const char* const kParamPvo[]       = {"fast_period", "slow_period", "signal_period"};
static const char* const kParamCrsi[]      = {"rsi_period", "streak_period", "rank_period"};
static const char* const kParamTtm[]       = {"bb_period", "bb_mult", "kc_period", "kc_mult", "momentum_period"};
static const char* const kParamCoppock[]   = {"wma_period", "long_roc_period", "short_roc_period"};
static const char* const kParamIchimoku[]  = {"tenkan", "kijun", "span_b"};
static const char* const kParamLR[]        = {"left", "right"};
static const char* const kParamDivNum[]    = {"left", "right", "lookback"};
static const char* const kParamDivNumDef[] = {NULL, NULL, "200"};
static const char* const kParamWedge[]     = {"left", "right", "lookback", "max_width_ratio"};
static const char* const kParamSaw[]       = {"swing_period", "apt", "use_adapt", "vol_bias", "atr_period", "min_apt", "max_apt"};
static const char* const kParamSawDef[]    = {"50", "20", "0", "10", "50", "5", "300"};
static const char* const kParamZigzag[]    = {"threshold", "n"};
static const char* const kParamStoch[]     = {"k_period", "d_period", "smooth_k"};
static const char* const kParamStochRsi[]  = {"rsi_period", "stoch_period", "smooth_k", "smooth_d"};
static const char* const kParamKeltner[]   = {"ema_period", "atr_mult", "atr_period"};
static const char* const kParamFastSlow[]  = {"fast_period", "slow_period"};
static const char* const kParamTsi[]       = {"long_period", "short_period"};
static const char* const kParamMassIdx[]   = {"sum_period", "ema_period"};
static const char* const kParamKst[]       = {"roc1_period", "roc2_period", "roc3_period", "roc4_period"};
static const char* const kParamStc[]       = {"fast_period", "slow_period", "cycle_period"};
static const char* const kParamPsar[]      = {"step", "max_step"};
static const char* const kParamAnchorVwap[]= {"anchor_bars"};

typedef struct {
    const char* indicator_name;
    cxta_param_spec spec;
} cxta_param_spec_entry;

static const cxta_param_spec_entry kParamSpecTable[] = {
    /* Single-period indicators */
    /* sma: params live in sma.h via descriptor->params */
    /* ema: params live in ema.h via descriptor->params */
    /* rsi: params live in rsi.h via descriptor->params */
    {"dema",                    {kParamPeriod,    1, NULL,             -1}},
    {"tema",                    {kParamPeriod,    1, NULL,             -1}},
    {"hma",                     {kParamPeriod,    1, NULL,             -1}},
    {"realized_volatility",     {kParamPeriod,    1, NULL,             -1}},
    {"linear_regression_slope", {kParamPeriod,    1, NULL,             -1}},
    {"linreg_angle",            {kParamPeriod,    1, NULL,             -1}},
    {"cci",                     {kParamPeriod,    1, NULL,             -1}},
    {"williams_r",              {kParamPeriod,    1, NULL,             -1}},
    {"donchian",                {kParamPeriod,    1, NULL,             -1}},
    {"trix",                    {kParamPeriod,    1, NULL,             -1}},
    {"dpo",                     {kParamPeriod,    1, NULL,             -1}},
    {"ease_of_movement",        {kParamPeriod,    1, NULL,             -1}},
    {"volume_sma",              {kParamPeriod,    1, NULL,             -1}},
    {"volume_ema",              {kParamPeriod,    1, NULL,             -1}},
    {"vwap",                    {kParamPeriod,    1, NULL,             -1}},
    {"mfi",                     {kParamPeriod,    1, NULL,             -1}},
    {"adx",                     {kParamPeriod,    1, NULL,             -1}},
    {"kama",                    {kParamPeriod,    1, NULL,             -1}},
    {"frama",                   {kParamPeriod,    1, NULL,             -1}},
    {"vidya",                   {kParamPeriod,    1, NULL,             -1}},
    {"fisher_transform",        {kParamPeriod,    1, NULL,             -1}},
    {"choppiness_index",        {kParamPeriod,    1, NULL,             -1}},
    {"ulcer_index",             {kParamPeriod,    1, NULL,             -1}},
    {"rvi",                     {kParamPeriod,    1, NULL,             -1}},
    {"cmf",                     {kParamPeriod,    1, NULL,             -1}},
    {"vortex",                  {kParamPeriod,    1, NULL,             -1}},
    {"aroon",                   {kParamPeriod,    1, NULL,             -1}},
    {"cmo",                     {kParamPeriod,    1, NULL,             -1}},
    {"price_channel",           {kParamPeriod,    1, NULL,             -1}},
    {"elder_force",             {kParamPeriod,    1, NULL,             -1}},
    /* "lookback" alias for single numeric arg */
    {"fvg",                     {kParamLookback,  1, NULL,             -1}},
    /* Two-param band indicators */
    /* bollinger: params live in bollinger.h via descriptor->params */
    {"supertrend",              {kParamBand,      2, NULL,             -1}},
    /* Other multi-param indicators */
    {"historical_volatility",   {kParamHistVol,   2, NULL,             -1}},
    {"volume_profile",          {kParamVolProf,   2, NULL,             -1}},
    {"mama",                    {kParamMama,      2, NULL,             -1}},
    {"dominant_cycle_period",   {kParamDomCycle,  2, NULL,             -1}},
    {"anchored_vwap",           {kParamAnchorVwap,1, NULL,             -1}},
    {"pvo",                     {kParamPvo,       3, NULL,             -1}},
    {"crsi",                    {kParamCrsi,      3, NULL,             -1}},
    {"ttm_squeeze",             {kParamTtm,       5, NULL,             -1}},
    {"coppock_curve",           {kParamCoppock,   3, NULL,             -1}},
    {"ichimoku",                {kParamIchimoku,  3, NULL,             -1}},
    /* left/right pattern indicators */
    {"trendline",               {kParamLR,        2, NULL,             -1}},
    {"swing_pivots",            {kParamLR,        2, NULL,             -1}},
    {"structure",               {kParamLR,        2, NULL,             -1}},
    {"order_block",             {kParamLR,        2, NULL,             -1}},
    {"liquidity",               {kParamLR,        2, NULL,             -1}},
    {"sfp",                     {kParamLR,        2, NULL,             -1}},
    /* divergence numeric params only (bridge prepends source_a, source_b) */
    {"divergence",              {kParamDivNum,    3, kParamDivNumDef,   2}},
    {"wedge",                   {kParamWedge,     4, NULL,             -1}},
    {"swing_anchor_vwap",       {kParamSaw,       7, kParamSawDef,      0}},
    /* zigzag: params live in zigzag.h via descriptor->params */
    {"stochastic",              {kParamStoch,     3, NULL,             -1}},
    {"stoch_rsi",               {kParamStochRsi,  4, NULL,             -1}},
    {"keltner",                 {kParamKeltner,   3, NULL,             -1}},
    {"awesome_oscillator",      {kParamFastSlow,  2, NULL,             -1}},
    {"chaikin_oscillator",      {kParamFastSlow,  2, NULL,             -1}},
    {"ppo",                     {kParamFastSlow,  2, NULL,             -1}},
    {"tsi",                     {kParamTsi,       2, NULL,             -1}},
    {"mass_index",              {kParamMassIdx,   2, NULL,             -1}},
    {"kst",                     {kParamKst,       4, NULL,             -1}},
    {"schaff_trend_cycle",      {kParamStc,       3, NULL,             -1}},
    {"parabolic_sar",           {kParamPsar,      2, NULL,             -1}},
};

const cxta_param_spec* cxta_indicator_param_spec_find(const char* name) {
    size_t i;
    if (!name) return NULL;
    for (i = 0; i < sizeof(kParamSpecTable) / sizeof(kParamSpecTable[0]); ++i) {
        if (strcmp(kParamSpecTable[i].indicator_name, name) == 0)
            return &kParamSpecTable[i].spec;
    }
    return NULL;
}

bool cxta_indicator_descriptor_supports_scalar_source(
    const cxta_indicator_descriptor* descriptor) {
    return descriptor != NULL &&
           (descriptor->flags & CXTA_INDICATOR_SCALAR_SOURCE) != 0u &&
           descriptor->scalar_source_min_args >= 0 &&
           descriptor->scalar_source_max_args >= 0;
}

bool cxta_indicator_field_auto_plot(const cxta_indicator_descriptor* descriptor,
                                    const cxta_field_descriptor* field) {
    return descriptor != NULL &&
           field != NULL &&
           field->auto_plot &&
           (descriptor->flags & CXTA_INDICATOR_REPAINTING) == 0u;
}

void cxta_name_sanitize_suffix(const char* name, char* out, size_t out_size) {
    size_t i;

    if (!out || out_size == 0u) return;
    out[0] = '\0';
    if (!name) return;

    for (i = 0u; name[i] != '\0' && i + 1u < out_size; ++i) {
        const char c = name[i];
        const int is_ident =
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '_';
        out[i] = is_ident ? c : '_';
    }
    out[i] = '\0';
}

int cxta_name_build_timeframe(const char* name, char* out, size_t out_size) {
    char suffix[256u];
    cxta_name_sanitize_suffix(name, suffix, sizeof(suffix));
    return snprintf(out, out_size, "%s_tf", suffix);
}

int cxta_name_build_source_aware(const char* smoothing_name,
                                 const char* source_name,
                                 char* out,
                                 size_t out_size) {
    char suffix[256u];
    cxta_name_sanitize_suffix(source_name, suffix, sizeof(suffix));
    return snprintf(out, out_size, "%s_src_%s", smoothing_name, suffix);
}
