/**
 * @file chaikin.c
 * @brief Chaikin Oscillator helpers.
 */

#include <cxta/indicators/chaikin.h>
#include <cxta/indicators/macros.h>
#include <cxta/ts/smoothing.h>
#include <limits.h>
#include <math.h>

static const cxta_scalar_plot_descriptor cxta_chaikin_scalar_plot =
    CXTA_SCALAR_PLOT("Chaikin Oscillator", "volume", "#22d3ee", "line", "volume",
                     "Accumulation/distribution momentum oscillator.",
                     "Use zero-line crosses and divergences to confirm volume-backed price pressure.");

static const cxta_indicator_plot_descriptor cxta_chaikin_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("chaikin_oscillator", cxta_chaikin_scalar_plot);

static int cxta_chaikin_descriptor_int_arg(const double* args,
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

static int cxta_chaikin_descriptor_period_arg(const double* args,
                                              size_t nargs,
                                              size_t index,
                                              int fallback) {
    return cxta_ts_clamp_period(
        cxta_chaikin_descriptor_int_arg(args, nargs, index, fallback));
}

static double cxta_chaikin_oscillator_descriptor_eval(const cxta_series_bar_view* view,
                                                      const double* args,
                                                      size_t nargs) {
    int fast = cxta_chaikin_descriptor_period_arg(args, nargs, 0u, 3);
    int slow = cxta_chaikin_descriptor_period_arg(args, nargs, 1u, 10);
    int swap_tmp;

    if (fast > slow) {
        swap_tmp = fast;
        fast = slow;
        slow = swap_tmp;
    }
    return cxta_chaikin(view, fast, slow);
}

const cxta_indicator_descriptor cxta_chaikin_oscillator_descriptor = {
    "chaikin_oscillator",
    2,
    2,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    sizeof(cxta_chaikin_state),
    NULL,
    0u,
    cxta_chaikin_oscillator_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_chaikin_oscillator_params,
    CXTA_ARRAY_COUNT(cxta_chaikin_oscillator_params),
    "volume",
    &cxta_chaikin_plot_descriptor,
};

static double cxta_chaikin_mfm(double high, double low, double close) {
    const double hl = high - low;
    if (hl <= 1e-12) return 0.0;
    return ((close - low) - (high - close)) / hl;
}

double cxta_chaikin_step(double high,
                         double low,
                         double close,
                         double volume,
                         int fast,
                         int slow,
                         cxta_chaikin_state* st) {
    if (!st) return 0.0;
    if (fast > slow) {
        const int tmp = fast;
        fast = slow;
        slow = tmp;
    }

    st->adl += cxta_chaikin_mfm(high, low, close) * volume;
    if (st->initialized == 0.0) {
        st->ema_fast = st->adl;
        st->ema_slow = st->adl;
        st->initialized = 1.0;
        return 0.0;
    }

    st->ema_fast = cxta_ts_ema_step(st->ema_fast, st->adl, fast);
    st->ema_slow = cxta_ts_ema_step(st->ema_slow, st->adl, slow);
    return st->ema_fast - st->ema_slow;
}

double cxta_chaikin(const cxta_series_bar_view* view, int fast, int slow) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    cxta_chaikin_state st = {0.0, 0.0, 0.0, 0.0};
    double out = 0.0;
    for (size_t i = 0; i <= idx; ++i) {
        out = cxta_chaikin_step(
            view->bars[i].high,
            view->bars[i].low,
            view->bars[i].close,
            view->bars[i].volume,
            fast,
            slow,
            &st);
    }
    return out;
}
