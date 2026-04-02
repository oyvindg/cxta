/**
 * @file descriptor.c
 * @brief Descriptor metadata for expression-facing indicators.
 */

#include <cxta/cxta.h>

#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

#define CXTA_ARRAY_COUNT(values) (sizeof(values) / sizeof((values)[0]))

#define CXTA_DESCRIPTOR_SCALAR_EX(name, min_args, max_args, state_sz, eval_scalar_fn)      \
    {                                                                                       \
        (name), (min_args), (max_args), -1, -1, -1,                                        \
        CXTA_INDICATOR_SCALAR, 0u, (state_sz), NULL, 0u,                                   \
        (eval_scalar_fn), NULL, NULL, NULL, NULL, NULL                                     \
    }

#define CXTA_DESCRIPTOR_SCALAR_FIELD_EX(name, min_args, max_args, state_sz, fields_array, eval_scalar_fn) \
    {                                                                                                      \
        (name), (min_args), (max_args), -1, -1, -1,                                                       \
        CXTA_INDICATOR_SCALAR, 0u, (state_sz), (fields_array), CXTA_ARRAY_COUNT(fields_array),            \
        (eval_scalar_fn), NULL, NULL, NULL, NULL, NULL                                                    \
    }

#define CXTA_DESCRIPTOR_SCALAR_SOURCE_EX(name, min_args, max_args, ss_min, ss_max, state_sz, eval_scalar_fn, eval_scalar_src_fn) \
    {                                                                                                                            \
        (name), (min_args), (max_args), (ss_min), (ss_max), -1,                                                                \
        CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_SCALAR_SOURCE, 0u, (state_sz), NULL, 0u,                                       \
        (eval_scalar_fn), NULL, (eval_scalar_src_fn), NULL, NULL, NULL                                                          \
    }

#define CXTA_DESCRIPTOR_SCALAR_SOURCE_STEP_EX(name, min_args, max_args, ss_min, ss_max, state_sz, eval_scalar_fn, eval_scalar_src_fn, step_scalar_fn) \
    {                                                                                                                                               \
        (name), (min_args), (max_args), (ss_min), (ss_max), -1,                                                                                   \
        CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_SCALAR_SOURCE, 0u, (state_sz), NULL, 0u,                                                          \
        (eval_scalar_fn), NULL, (eval_scalar_src_fn), NULL, (step_scalar_fn), NULL                                                                \
    }

#define CXTA_DESCRIPTOR_SCALAR_SOURCE_DYNAMIC_STEP_EX(name, min_args, max_args, ss_min, ss_max, state_slots_fn, eval_scalar_fn, eval_scalar_src_fn, step_scalar_fn) \
    {                                                                                                                                                           \
        (name), (min_args), (max_args), (ss_min), (ss_max), -1,                                                                                               \
        CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_SCALAR_SOURCE, 0u, 0u, NULL, 0u,                                                                              \
        (eval_scalar_fn), NULL, (eval_scalar_src_fn), (state_slots_fn), (step_scalar_fn), NULL                                                               \
    }

#define CXTA_DESCRIPTOR_STRUCT_EX(name, min_args, max_args, primary_index, type, state_sz, fields_array, eval_struct_fn) \
    {                                                                                                                     \
        (name), (min_args), (max_args), -1, -1, (primary_index),                                                         \
        CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT, sizeof(type), (state_sz),                                         \
        (fields_array), CXTA_ARRAY_COUNT(fields_array),                                                                   \
        NULL, (eval_struct_fn), NULL, NULL, NULL, NULL                                                                    \
    }

#define CXTA_DESCRIPTOR_STRUCT_STEP_EX(name, min_args, max_args, primary_index, type, state_sz, fields_array, eval_struct_fn, step_struct_fn) \
    {                                                                                                                                           \
        (name), (min_args), (max_args), -1, -1, (primary_index),                                                                               \
        CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT, sizeof(type), (state_sz),                                                               \
        (fields_array), CXTA_ARRAY_COUNT(fields_array),                                                                                         \
        NULL, (eval_struct_fn), NULL, NULL, NULL, (step_struct_fn)                                                                              \
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

static double cxta_desc_eval_sma_scalar_src(const cxta_series_scalar_view* source,
                                            const double* args,
                                            size_t nargs) {
    size_t idx;
    size_t start;
    size_t window;
    size_t i;
    double sum = 0.0;

    if (!source || !cxta_series_scalar_view_valid(source)) return 0.0;
    idx = cxta_series_clamp_index(source->size, source->index);
    window = (size_t)cxta_descriptor_period_arg(args, nargs, 0u, 20);
    if (window > idx + 1u) window = idx + 1u;
    start = idx + 1u - window;
    for (i = start; i <= idx; ++i) sum += source->values[i];
    return cxta_sma_from_sum(sum, window);
}

static size_t cxta_desc_state_slots_sma(const double* args,
                                        size_t nargs) {
    return 3u + (size_t)cxta_descriptor_period_arg(args, nargs, 0u, 20);
}

static double cxta_desc_step_sma(double sample,
                                 double prev_sample,
                                 const double* args,
                                 size_t nargs,
                                 void* state) {
    double* values = (double*)state;
    const size_t period = (size_t)cxta_descriptor_period_arg(args, nargs, 0u, 20);
    size_t count_value;
    size_t pos_value;

    (void)prev_sample;
    if (!values || period == 0u) return 0.0;

    count_value = (size_t)values[1];
    pos_value = (size_t)values[2];

    if (count_value < period) {
        values[3u + count_value] = sample;
        values[0] += sample;
        values[1] = (double)(count_value + 1u);
        values[2] = (double)((count_value + 1u) % period);
        return cxta_sma_from_sum(values[0], count_value + 1u);
    }

    values[0] += sample - values[3u + pos_value];
    values[3u + pos_value] = sample;
    values[2] = (double)((pos_value + 1u) % period);
    return cxta_sma_from_sum(values[0], period);
}

static double cxta_desc_eval_ema_scalar_src(const cxta_series_scalar_view* source,
                                            const double* args,
                                            size_t nargs) {
    size_t idx;
    size_t i;
    int period;
    cxta_ema_state st = {0.0, 0.0};
    double out = 0.0;

    if (!source || !cxta_series_scalar_view_valid(source)) return 0.0;
    idx = cxta_series_clamp_index(source->size, source->index);
    period = cxta_descriptor_period_arg(args, nargs, 0u, 20);
    for (i = 0; i <= idx; ++i) {
        out = cxta_ema_step(source->values[i], period, &st);
    }
    return out;
}

static double cxta_desc_eval_rma_scalar_src(const cxta_series_scalar_view* source,
                                            const double* args,
                                            size_t nargs) {
    size_t idx;
    size_t i;
    int period;
    cxta_rma_state st = {0.0, 0.0};
    double out = 0.0;

    if (!source || !cxta_series_scalar_view_valid(source)) return 0.0;
    idx = cxta_series_clamp_index(source->size, source->index);
    period = cxta_descriptor_period_arg(args, nargs, 0u, 20);
    for (i = 0; i <= idx; ++i) {
        out = cxta_rma_step(source->values[i], period, &st);
    }
    return out;
}

typedef struct {
    double avg_gain;
    double avg_loss;
    double prev_value;
    double samples_seen;
} cxta_desc_rsi_state;

static double cxta_desc_rsi_from_averages(double avg_gain,
                                          double avg_loss) {
    if (avg_loss < 1e-12) return 100.0;
    return 100.0 - (100.0 / (1.0 + (avg_gain / avg_loss)));
}

static double cxta_desc_step_ema(double close,
                                 double prev_close,
                                 const double* args,
                                 size_t nargs,
                                 void* state) {
    (void)prev_close;
    return cxta_ema_step(
        close,
        cxta_descriptor_period_arg(args, nargs, 0u, 20),
        (cxta_ema_state*)state);
}

static double cxta_desc_step_rma(double close,
                                 double prev_close,
                                 const double* args,
                                 size_t nargs,
                                 void* state) {
    (void)prev_close;
    return cxta_rma_step(
        close,
        cxta_descriptor_period_arg(args, nargs, 0u, 20),
        (cxta_rma_state*)state);
}

static double cxta_desc_step_rsi(double close,
                                 double prev_close,
                                 const double* args,
                                 size_t nargs,
                                 void* state) {
    cxta_desc_rsi_state* st = (cxta_desc_rsi_state*)state;
    const int period = cxta_descriptor_period_arg(args, nargs, 0u, 14);
    double diff;

    (void)prev_close;
    if (!st) return 50.0;

    if (st->samples_seen <= 0.0) {
        st->prev_value = close;
        st->samples_seen = 1.0;
        return 50.0;
    }

    diff = close - st->prev_value;
    if (st->samples_seen <= (double)period) {
        if (diff > 0.0) st->avg_gain += diff;
        else st->avg_loss -= diff;
        st->prev_value = close;
        st->samples_seen += 1.0;

        if (st->samples_seen <= (double)period) return 50.0;

        st->avg_gain /= (double)period;
        st->avg_loss /= (double)period;
        return cxta_desc_rsi_from_averages(st->avg_gain, st->avg_loss);
    }

    cxta_ts_update_gain_loss(&st->avg_gain, &st->avg_loss, diff, period);
    st->prev_value = close;
    return cxta_desc_rsi_from_averages(st->avg_gain, st->avg_loss);
}

static double cxta_desc_eval_rsi_scalar_src(const cxta_series_scalar_view* source,
                                            const double* args,
                                            size_t nargs) {
    size_t idx;
    size_t i;
    int period;
    double avg_gain = 0.0;
    double avg_loss = 0.0;

    if (!source || !cxta_series_scalar_view_valid(source)) return 50.0;
    idx = cxta_series_clamp_index(source->size, source->index);
    period = cxta_descriptor_period_arg(args, nargs, 0u, 14);
    if (idx == 0u || idx < (size_t)period) return 50.0;

    for (i = 1u; i <= (size_t)period; ++i) {
        double gain = 0.0;
        double loss = 0.0;
        cxta_ts_gain_loss(source->values[i] - source->values[i - 1u], &gain, &loss);
        avg_gain += gain;
        avg_loss += loss;
    }
    avg_gain /= (double)period;
    avg_loss /= (double)period;

    for (i = (size_t)period + 1u; i <= idx; ++i) {
        cxta_ts_update_gain_loss(
            &avg_gain,
            &avg_loss,
            source->values[i] - source->values[i - 1u],
            period);
    }

    if (avg_loss < 1e-12) return 100.0;
    return 100.0 - (100.0 / (1.0 + (avg_gain / avg_loss)));
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

CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_sma, cxta_sma, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_ema, cxta_ema, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_atr, cxta_atr, 14)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_rsi, cxta_rsi, 14)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_volume_sma, cxta_volume_sma, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_wma, cxta_wma, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_dema, cxta_dema, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_tema, cxta_tema, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_hma, cxta_hma, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_roc, cxta_roc, 10)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_linreg_slope, cxta_linreg_slope, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_linreg_angle, cxta_linreg_angle, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_stddev, cxta_stddev, 20)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_rvol, cxta_rvol, 20)
CXTA_WRAP_BAR_SCALAR_0(cxta_desc_eval_truerange, cxta_truerange)
CXTA_WRAP_BAR_SCALAR_0(cxta_desc_eval_typical, cxta_typical)
CXTA_WRAP_BAR_SCALAR_0(cxta_desc_eval_median_price, cxta_median_price)
CXTA_WRAP_BAR_SCALAR_0(cxta_desc_eval_weighted_close, cxta_wclose)
CXTA_WRAP_BAR_SCALAR_0(cxta_desc_eval_obv, cxta_obv)
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_zscore, cxta_zscore, 20)

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
CXTA_WRAP_BAR_SCALAR_1I(cxta_desc_eval_rma, cxta_rma, 20)
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

static void cxta_desc_eval_macd(const cxta_series_bar_view* view,
                                const double* args,
                                size_t nargs,
                                void* out) {
    int fast = cxta_descriptor_period_arg(args, nargs, 0u, 12);
    int slow = cxta_descriptor_period_arg(args, nargs, 1u, 26);
    int signal = cxta_descriptor_period_arg(args, nargs, 2u, 9);
    int swap_tmp;
    cxta_macd_output value;

    if (fast > slow) {
        swap_tmp = fast;
        fast = slow;
        slow = swap_tmp;
    }
    value = cxta_macd(view, fast, slow, signal);
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

CXTA_WRAP_BAR_STRUCT_1I_1D(cxta_desc_eval_bollinger, cxta_bollinger_output, cxta_bollinger, 20, 2.0)
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

static void cxta_desc_eval_zigzag(const cxta_series_bar_view* view,
                                  const double* args,
                                  size_t nargs,
                                  void* out) {
    const double threshold = cxta_descriptor_double_arg(args, nargs, 0u, 0.03);
    const int n = cxta_descriptor_clamp_int_arg(args, nargs, 1u, 0, 0, INT_MAX);
    const cxta_zigzag_output value = cxta_zigzag(view, threshold, n);
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

static const cxta_field_descriptor kMacdFields[] = {
    CXTA_FIELD("line", offsetof(cxta_macd_output, line)),
    CXTA_FIELD("signal", offsetof(cxta_macd_output, signal)),
    CXTA_FIELD("histogram", offsetof(cxta_macd_output, histogram)),
};

static const cxta_field_descriptor kBollingerFields[] = {
    CXTA_FIELD("upper", offsetof(cxta_bollinger_output, upper)),
    CXTA_FIELD("lower", offsetof(cxta_bollinger_output, lower)),
    CXTA_FIELD("middle", offsetof(cxta_bollinger_output, middle)),
    CXTA_FIELD("percentB", offsetof(cxta_bollinger_output, percent_b)),
    CXTA_FIELD("width", offsetof(cxta_bollinger_output, bandwidth)),
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

static const cxta_field_descriptor kZigZagFields[] = {
    CXTA_FIELD_HIDDEN("high", offsetof(cxta_zigzag_output, high)),
    CXTA_FIELD_HIDDEN("low", offsetof(cxta_zigzag_output, low)),
    CXTA_FIELD("line", offsetof(cxta_zigzag_output, line)),
    CXTA_FIELD_HIDDEN("pivot_index", offsetof(cxta_zigzag_output, pivot_index)),
    CXTA_FIELD_HIDDEN("active", offsetof(cxta_zigzag_output, active)),
    CXTA_FIELD_HIDDEN("active_index", offsetof(cxta_zigzag_output, active_index)),
    CXTA_FIELD_HIDDEN("direction", offsetof(cxta_zigzag_output, direction)),
    CXTA_FIELD_HIDDEN("last", offsetof(cxta_zigzag_output, last)),
    CXTA_FIELD_HIDDEN("is_high", offsetof(cxta_zigzag_output, is_high)),
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

static const cxta_indicator_descriptor kDescriptors[] = {
    CXTA_DESCRIPTOR_SCALAR_SOURCE_DYNAMIC_STEP_EX("sma", 1, 1, 1, 1, cxta_desc_state_slots_sma, cxta_desc_eval_sma, cxta_desc_eval_sma_scalar_src, cxta_desc_step_sma),
    CXTA_DESCRIPTOR_SCALAR_SOURCE_STEP_EX("ema", 1, 1, 1, 1, sizeof(cxta_ema_state), cxta_desc_eval_ema, cxta_desc_eval_ema_scalar_src, cxta_desc_step_ema),
    CXTA_DESCRIPTOR_SCALAR_EX("atr", 1, 1, sizeof(cxta_atr_state), cxta_desc_eval_atr),
    CXTA_DESCRIPTOR_SCALAR_SOURCE_STEP_EX("rsi", 1, 1, 1, 1, sizeof(cxta_desc_rsi_state), cxta_desc_eval_rsi, cxta_desc_eval_rsi_scalar_src, cxta_desc_step_rsi),
    CXTA_DESCRIPTOR_STRUCT_EX("macd", 3, 3, 0, cxta_macd_output, sizeof(cxta_macd_state), kMacdFields, cxta_desc_eval_macd),
    CXTA_DESCRIPTOR_STRUCT_EX("bollinger", 2, 2, 2, cxta_bollinger_output, 0u, kBollingerFields, cxta_desc_eval_bollinger),
    CXTA_DESCRIPTOR_STRUCT_EX("adx", 1, 1, 0, cxta_adx_output, sizeof(cxta_adx_state), kAdxFields, cxta_desc_eval_adx),
    CXTA_DESCRIPTOR_STRUCT_EX("supertrend", 2, 2, 0, cxta_supertrend_output, sizeof(cxta_supertrend_state), kSupertrendFields, cxta_desc_eval_supertrend),
    CXTA_DESCRIPTOR_STRUCT_EX("donchian", 1, 1, 2, cxta_channel_output, 0u, kChannelFields, cxta_desc_eval_donchian),
    CXTA_DESCRIPTOR_STRUCT_EX("keltner", 2, 3, 2, cxta_channel_output, 0u, kChannelFields, cxta_desc_eval_keltner),
    CXTA_DESCRIPTOR_STRUCT_EX("price_channel", 1, 1, 2, cxta_channel_output, 0u, kChannelFields, cxta_desc_eval_price_channel),
    CXTA_DESCRIPTOR_SCALAR_EX("volume_sma", 1, 1, 0u, cxta_desc_eval_volume_sma),
    CXTA_DESCRIPTOR_SCALAR_EX("wma", 1, 1, 0u, cxta_desc_eval_wma),
    CXTA_DESCRIPTOR_SCALAR_EX("dema", 1, 1, sizeof(cxta_dema_state), cxta_desc_eval_dema),
    CXTA_DESCRIPTOR_SCALAR_EX("tema", 1, 1, sizeof(cxta_tema_state), cxta_desc_eval_tema),
    CXTA_DESCRIPTOR_SCALAR_EX("hma", 1, 1, 0u, cxta_desc_eval_hma),
    CXTA_DESCRIPTOR_SCALAR_EX("roc", 1, 1, 0u, cxta_desc_eval_roc),
    CXTA_DESCRIPTOR_SCALAR_EX("linear_regression_slope", 1, 1, 0u, cxta_desc_eval_linreg_slope),
    CXTA_DESCRIPTOR_SCALAR_EX("linreg_angle", 1, 1, 0u, cxta_desc_eval_linreg_angle),
    CXTA_DESCRIPTOR_SCALAR_EX("stddev", 1, 1, 0u, cxta_desc_eval_stddev),
    CXTA_DESCRIPTOR_SCALAR_EX("historical_volatility", 1, 2, 0u, cxta_desc_eval_historical_volatility),
    CXTA_DESCRIPTOR_SCALAR_EX("realized_volatility", 1, 1, 0u, cxta_desc_eval_rvol),
    CXTA_DESCRIPTOR_SCALAR_EX("true_range", 0, 0, 0u, cxta_desc_eval_truerange),
    CXTA_DESCRIPTOR_SCALAR_EX("typical_price", 0, 0, 0u, cxta_desc_eval_typical),
    CXTA_DESCRIPTOR_SCALAR_EX("median_price", 0, 0, 0u, cxta_desc_eval_median_price),
    CXTA_DESCRIPTOR_SCALAR_EX("weighted_close", 0, 0, 0u, cxta_desc_eval_weighted_close),
    CXTA_DESCRIPTOR_SCALAR_EX("obv", 0, 0, sizeof(cxta_obv_state), cxta_desc_eval_obv),
    CXTA_DESCRIPTOR_SCALAR_EX("zscore", 1, 1, 0u, cxta_desc_eval_zscore),
    CXTA_DESCRIPTOR_SCALAR_EX("awesome_oscillator", 2, 2, 0u, cxta_desc_eval_awesome_oscillator),
    CXTA_DESCRIPTOR_SCALAR_EX("bop", 0, 0, 0u, cxta_desc_eval_bop),
    CXTA_DESCRIPTOR_SCALAR_EX("ease_of_movement", 1, 1, 0u, cxta_desc_eval_ease_of_movement),
    CXTA_DESCRIPTOR_SCALAR_EX("volume_ema", 1, 1, sizeof(cxta_volume_ema_state), cxta_desc_eval_volume_ema),
    CXTA_DESCRIPTOR_SCALAR_SOURCE_STEP_EX("rma", 1, 1, 1, 1, sizeof(cxta_rma_state), cxta_desc_eval_rma, cxta_desc_eval_rma_scalar_src, cxta_desc_step_rma),
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
    {
        "zigzag", 1, 2, -1, -1, 0,
        CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT | CXTA_INDICATOR_REPAINTING,
        sizeof(cxta_zigzag_output), 0u,
        kZigZagFields, CXTA_ARRAY_COUNT(kZigZagFields),
        NULL, cxta_desc_eval_zigzag, NULL, NULL, NULL
    },
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

const cxta_indicator_descriptor* cxta_indicator_descriptors(size_t* count) {
    if (count) *count = CXTA_ARRAY_COUNT(kDescriptors);
    return kDescriptors;
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
