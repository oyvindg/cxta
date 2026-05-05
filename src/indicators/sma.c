/**
 * @file sma.c
 * @brief Simple moving average helpers.
 */

#include <limits.h>
#include <math.h>

#include <cxta/indicators/sma.h>
#include <cxta/indicators/macros.h>
#include <cxta/ts/smoothing.h>

static const cxta_scalar_plot_descriptor cxta_sma_scalar_plot =
    CXTA_SCALAR_PLOT("SMA", "price", "#93c5fd", "line", "price",
                     "Simple Moving Average price overlay.",
                     "Use as trend baseline, support/resistance reference, or crossover component.");

static const cxta_indicator_plot_descriptor cxta_sma_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("sma", cxta_sma_scalar_plot);

static int cxta_sma_descriptor_period_arg(const double* args,
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

static double cxta_sma_descriptor_eval(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs) {
    return cxta_sma(view, cxta_sma_descriptor_period_arg(args, nargs, 0u, 20));
}

static double cxta_sma_descriptor_eval_scalar_src(const cxta_series_scalar_view* source,
                                                  const double* args,
                                                  size_t nargs) {
    size_t idx;
    size_t start;
    size_t window;
    size_t i;
    double sum = 0.0;

    if (!source || !cxta_series_scalar_view_valid(source)) return 0.0;
    idx = cxta_series_clamp_index(source->size, source->index);
    window = (size_t)cxta_sma_descriptor_period_arg(args, nargs, 0u, 20);
    if (window > idx + 1u) window = idx + 1u;
    start = idx + 1u - window;
    for (i = start; i <= idx; ++i) sum += source->values[i];
    return cxta_sma_from_sum(sum, window);
}

static size_t cxta_sma_descriptor_state_slots(const double* args,
                                              size_t nargs) {
    return 3u + (size_t)cxta_sma_descriptor_period_arg(args, nargs, 0u, 20);
}

static double cxta_sma_descriptor_step(double sample,
                                       double prev_sample,
                                       const double* args,
                                       size_t nargs,
                                       void* state) {
    double* values = (double*)state;
    const size_t period = (size_t)cxta_sma_descriptor_period_arg(args, nargs, 0u, 20);
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

const cxta_indicator_descriptor cxta_sma_descriptor = {
    "sma",
    1,
    1,
    1,
    1,
    -1,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_SCALAR_SOURCE,
    0u,
    0u,
    NULL,
    0u,
    cxta_sma_descriptor_eval,
    NULL,
    cxta_sma_descriptor_eval_scalar_src,
    cxta_sma_descriptor_state_slots,
    cxta_sma_descriptor_step,
    NULL,
    cxta_sma_params,
    CXTA_ARRAY_COUNT(cxta_sma_params),
    "price",
    &cxta_sma_plot_descriptor,
};

double cxta_sma_from_sum(double sum, size_t window) {
    if (window == 0u) return 0.0;
    return sum / (double)window;
}

double cxta_sma(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t window = (p < (idx + 1u)) ? p : (idx + 1u);
    const size_t start = idx + 1u - window;

    double sum = 0.0;
    for (size_t i = start; i <= idx; ++i) {
        sum += view->bars[i].close;
    }
    return cxta_sma_from_sum(sum, window);
}
