/**
 * @file rma.c
 * @brief Wilder RMA helpers.
 */

#include <limits.h>
#include <math.h>

#include <cxta/indicators/rma.h>
#include <cxta/indicators/macros.h>
#include <cxta/ts/smoothing.h>

static const cxta_scalar_plot_descriptor cxta_rma_scalar_plot =
    CXTA_SCALAR_PLOT("RMA", "price", "#60a5fa", "line", "price",
                     "Wilder-style moving average price overlay.",
                     "Use as a smoother trend baseline or as Wilder smoothing for source-aware expressions.");

static const cxta_indicator_plot_descriptor cxta_rma_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("rma", cxta_rma_scalar_plot);

static int cxta_rma_descriptor_period_arg(const double* args,
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

static double cxta_rma_descriptor_eval(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs) {
    return cxta_rma(view, cxta_rma_descriptor_period_arg(args, nargs, 0u, 20));
}

static double cxta_rma_descriptor_eval_scalar_src(const cxta_series_scalar_view* source,
                                                  const double* args,
                                                  size_t nargs) {
    size_t idx;
    size_t i;
    int period;
    cxta_rma_state st = {0.0, 0.0};
    double out = 0.0;

    if (!source || !cxta_series_scalar_view_valid(source)) return 0.0;
    idx = cxta_series_clamp_index(source->size, source->index);
    period = cxta_rma_descriptor_period_arg(args, nargs, 0u, 20);
    for (i = 0; i <= idx; ++i) {
        out = cxta_rma_step(source->values[i], period, &st);
    }
    return out;
}

static double cxta_rma_descriptor_step(double close,
                                       double prev_close,
                                       const double* args,
                                       size_t nargs,
                                       void* state) {
    (void)prev_close;
    return cxta_rma_step(
        close,
        cxta_rma_descriptor_period_arg(args, nargs, 0u, 20),
        (cxta_rma_state*)state);
}

const cxta_indicator_descriptor cxta_rma_descriptor = {
    "rma",
    1,
    1,
    1,
    1,
    -1,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_SCALAR_SOURCE,
    0u,
    sizeof(cxta_rma_state),
    NULL,
    0u,
    cxta_rma_descriptor_eval,
    NULL,
    cxta_rma_descriptor_eval_scalar_src,
    NULL,
    cxta_rma_descriptor_step,
    NULL,
    cxta_rma_params,
    CXTA_ARRAY_COUNT(cxta_rma_params),
    "price",
    &cxta_rma_plot_descriptor,
};

double cxta_rma_step(double sample, int period, cxta_rma_state* st) {
    if (!st) return 0.0;
    if (st->initialized == 0.0) {
        st->value = sample;
        st->initialized = 1.0;
        return st->value;
    }
    st->value = cxta_ts_wilder_step(st->value, sample, period);
    return st->value;
}

double cxta_rma(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    cxta_rma_state st = {0.0, 0.0};
    double out = 0.0;
    for (size_t i = 0; i <= idx; ++i) {
        out = cxta_rma_step(view->bars[i].close, period, &st);
    }
    return out;
}
