/**
 * @file pvo.c
 * @brief Percentage Volume Oscillator helpers.
 */

#include <cxta/indicators/pvo.h>
#include <cxta/ts/smoothing.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

static const cxta_field_descriptor cxta_pvo_fields[] = {
    {"line", offsetof(cxta_pvo_output, line), true},
    {"signal", offsetof(cxta_pvo_output, signal), true},
    {"histogram", offsetof(cxta_pvo_output, histogram), true},
};

static int cxta_pvo_descriptor_int_arg(const double* args,
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

static int cxta_pvo_descriptor_period_arg(const double* args,
                                          size_t nargs,
                                          size_t index,
                                          int fallback) {
    return cxta_ts_clamp_period(cxta_pvo_descriptor_int_arg(args, nargs, index, fallback));
}

static void cxta_pvo_descriptor_eval(const cxta_series_bar_view* view,
                                     const double* args,
                                     size_t nargs,
                                     void* out) {
    int fast = cxta_pvo_descriptor_period_arg(args, nargs, 0u, 12);
    int slow = cxta_pvo_descriptor_period_arg(args, nargs, 1u, 26);
    int signal = cxta_pvo_descriptor_period_arg(args, nargs, 2u, 9);
    int swap_tmp;
    cxta_pvo_output value;

    if (fast > slow) {
        swap_tmp = fast;
        fast = slow;
        slow = swap_tmp;
    }
    value = cxta_pvo(view, fast, slow, signal);
    if (out) memcpy(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_pvo_descriptor = {
    "pvo",
    2,
    3,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_pvo_output),
    sizeof(cxta_pvo_state),
    cxta_pvo_fields,
    CXTA_ARRAY_COUNT(cxta_pvo_fields),
    NULL,
    cxta_pvo_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_pvo_params,
    CXTA_ARRAY_COUNT(cxta_pvo_params),
};

cxta_pvo_output cxta_pvo_step(double volume,
                              int fast,
                              int slow,
                              int signal,
                              cxta_pvo_state* st) {
    cxta_pvo_output out = {0.0, 0.0, 0.0};
    if (!st) return out;
    if (fast > slow) {
        const int tmp = fast;
        fast = slow;
        slow = tmp;
    }

    if (st->initialized == 0.0) {
        st->ema_fast = volume;
        st->ema_slow = volume;
        st->signal = 0.0;
        st->initialized = 1.0;
        return out;
    }

    st->ema_fast = cxta_ts_ema_step(st->ema_fast, volume, fast);
    st->ema_slow = cxta_ts_ema_step(st->ema_slow, volume, slow);
    if (fabs(st->ema_slow) > 1e-12) {
        out.line = 100.0 * ((st->ema_fast - st->ema_slow) / st->ema_slow);
    }
    st->signal = cxta_ts_ema_step(st->signal, out.line, signal);
    out.signal = st->signal;
    out.histogram = out.line - out.signal;
    return out;
}

cxta_pvo_output cxta_pvo(const cxta_series_bar_view* view, int fast, int slow, int signal) {
    cxta_pvo_output out = {0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    cxta_pvo_state st = {0.0, 0.0, 0.0, 0.0};
    for (size_t i = 0; i <= idx; ++i) {
        out = cxta_pvo_step(view->bars[i].volume, fast, slow, signal, &st);
    }
    return out;
}
