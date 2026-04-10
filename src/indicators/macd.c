/**
 * @file macd.c
 * @brief MACD helpers.
 */

#include <limits.h>
#include <math.h>
#include <string.h>

#include <cxta/indicators/descriptor.h>
#include <cxta/indicators/macd.h>
#include <cxta/ts/smoothing.h>

const cxta_field_descriptor cxta_macd_descriptor_fields[3] = {
    {"line", offsetof(cxta_macd_output, line), true},
    {"signal", offsetof(cxta_macd_output, signal), true},
    {"histogram", offsetof(cxta_macd_output, histogram), true},
};

static int cxta_macd_descriptor_int_arg(const double* args,
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

static int cxta_macd_descriptor_period_arg(const double* args,
                                           size_t nargs,
                                           size_t index,
                                           int fallback) {
    return cxta_ts_clamp_period(
        cxta_macd_descriptor_int_arg(args, nargs, index, fallback));
}

void cxta_macd_descriptor_eval(const cxta_series_bar_view* view,
                               const double* args,
                               size_t nargs,
                               void* out);

const cxta_indicator_descriptor cxta_macd_descriptor = {
    "macd",
    3,
    3,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_macd_output),
    sizeof(cxta_macd_state),
    cxta_macd_descriptor_fields,
    3u,
    NULL,
    cxta_macd_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_macd_params,
    CXTA_ARRAY_COUNT(cxta_macd_params),
};

void cxta_macd_descriptor_eval(const cxta_series_bar_view* view,
                               const double* args,
                               size_t nargs,
                               void* out) {
    int fast = cxta_macd_descriptor_period_arg(args, nargs, 0u, 12);
    int slow = cxta_macd_descriptor_period_arg(args, nargs, 1u, 26);
    int signal = cxta_macd_descriptor_period_arg(args, nargs, 2u, 9);
    int swap_tmp;
    cxta_macd_output value;

    if (!out) return;

    if (fast > slow) {
        swap_tmp = fast;
        fast = slow;
        slow = swap_tmp;
    }
    value = cxta_macd(view, fast, slow, signal);
    memcpy(out, &value, sizeof(value));
}

cxta_macd_output cxta_macd_step(double close,
                                int fast,
                                int slow,
                                int signal,
                                cxta_macd_state* st) {
    cxta_macd_output out = {0.0, 0.0, 0.0};
    if (!st) return out;

    st->ema_fast = cxta_ts_ema_step(st->ema_fast, close, fast);
    st->ema_slow = cxta_ts_ema_step(st->ema_slow, close, slow);
    out.line = st->ema_fast - st->ema_slow;
    st->signal = cxta_ts_ema_step(st->signal, out.line, signal);
    out.signal = st->signal;
    out.histogram = out.line - out.signal;
    return out;
}

cxta_macd_output cxta_macd(const cxta_series_bar_view* view,
                           int fast,
                           int slow,
                           int signal) {
    cxta_macd_output out = {0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    cxta_macd_state st = {
        view->bars[0].close,
        view->bars[0].close,
        0.0,
    };
    for (size_t i = 1; i <= view->index; ++i) {
        out = cxta_macd_step(view->bars[i].close, fast, slow, signal, &st);
    }
    return out;
}
