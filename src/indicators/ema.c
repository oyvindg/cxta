/**
 * @file ema.c
 * @brief Exponential moving average helpers.
 */

#include <limits.h>
#include <math.h>

#include <cxta/indicators/ema.h>
#include <cxta/indicators/macros.h>
#include <cxta/ts/smoothing.h>

static const cxta_scalar_plot_descriptor cxta_ema_scalar_plot =
    CXTA_SCALAR_PLOT("EMA", "price", "#38bdf8", "line", "price",
                     "Exponential Moving Average price overlay.",
                     "Use slope and price/MA crosses to read trend and pullbacks.");

static const cxta_indicator_plot_descriptor cxta_ema_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("ema", cxta_ema_scalar_plot);

static int cxta_ema_descriptor_period_arg(const double* args,
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

static double cxta_ema_descriptor_eval(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs) {
    return cxta_ema(view, cxta_ema_descriptor_period_arg(args, nargs, 0u, 20));
}

static double cxta_ema_descriptor_eval_scalar_src(const cxta_series_scalar_view* source,
                                                  const double* args,
                                                  size_t nargs) {
    size_t idx;
    size_t i;
    int period;
    cxta_ema_state st = {0.0, 0.0};
    double out = 0.0;

    if (!source || !cxta_series_scalar_view_valid(source)) return 0.0;
    idx = cxta_series_clamp_index(source->size, source->index);
    period = cxta_ema_descriptor_period_arg(args, nargs, 0u, 20);
    for (i = 0u; i <= idx; ++i) {
        out = cxta_ema_step(source->values[i], period, &st);
    }
    return out;
}

static double cxta_ema_descriptor_step(double close,
                                       double prev_close,
                                       const double* args,
                                       size_t nargs,
                                       void* state) {
    (void)prev_close;
    return cxta_ema_step(
        close,
        cxta_ema_descriptor_period_arg(args, nargs, 0u, 20),
        (cxta_ema_state*)state);
}

const cxta_indicator_descriptor cxta_ema_descriptor = {
    "ema",
    1,
    1,
    1,
    1,
    -1,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_SCALAR_SOURCE,
    0u,
    sizeof(cxta_ema_state),
    NULL,
    0u,
    cxta_ema_descriptor_eval,
    NULL,
    cxta_ema_descriptor_eval_scalar_src,
    NULL,
    cxta_ema_descriptor_step,
    NULL,
    cxta_ema_params,
    CXTA_ARRAY_COUNT(cxta_ema_params),
    "price",
    &cxta_ema_plot_descriptor,
};

double cxta_ema_step(double close, int period, cxta_ema_state* st) {
    if (!st) return 0.0;
    if (st->initialized == 0.0) {
        st->value = close;
        st->initialized = 1.0;
        return st->value;
    }
    st->value = cxta_ts_ema_step(st->value, close, period);
    return st->value;
}

double cxta_ema(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const int p = cxta_ts_clamp_period(period);
    double value = view->bars[0].close;
    for (size_t i = 1; i <= view->index; ++i) {
        value = cxta_ts_ema_step(value, view->bars[i].close, p);
    }
    return value;
}
