/**
 * @file tsi.c
 * @brief True Strength Index helpers.
 */

#include <cxta/indicators/tsi.h>
#include <cxta/indicators/macros.h>
#include <cxta/ts/smoothing.h>
#include <limits.h>
#include <math.h>

static const cxta_scalar_plot_descriptor cxta_tsi_scalar_plot =
    CXTA_SCALAR_PLOT("TSI", "momentum", "#22d3ee", "line", "momentum",
                     "True Strength Index double-smoothed momentum oscillator.",
                     "Use sign, slope, and turns for smoothed momentum confirmation.");

static const cxta_indicator_plot_descriptor cxta_tsi_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("tsi", cxta_tsi_scalar_plot);

static int cxta_tsi_descriptor_int_arg(const double* args,
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

static int cxta_tsi_descriptor_period_arg(const double* args,
                                          size_t nargs,
                                          size_t index,
                                          int fallback) {
    return cxta_ts_clamp_period(cxta_tsi_descriptor_int_arg(args, nargs, index, fallback));
}

static double cxta_tsi_descriptor_eval(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs) {
    const int long_period = cxta_tsi_descriptor_period_arg(args, nargs, 0u, 25);
    const int short_period = cxta_tsi_descriptor_period_arg(args, nargs, 1u, 13);
    return cxta_tsi(view, long_period, short_period);
}

const cxta_indicator_descriptor cxta_tsi_descriptor = {
    "tsi",
    2,
    2,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    sizeof(cxta_tsi_state),
    NULL,
    0u,
    cxta_tsi_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_tsi_params,
    CXTA_ARRAY_COUNT(cxta_tsi_params),
    "momentum",
    &cxta_tsi_plot_descriptor,
};

double cxta_tsi_step(double close, int long_period, int short_period, cxta_tsi_state* st) {
    if (!st) return 0.0;
    if (st->initialized == 0.0) {
        st->prev_close = close;
        st->initialized = 1.0;
        return 0.0;
    }

    {
        const double m = close - st->prev_close;
        const double am = fabs(m);
        st->ema_m1 = cxta_ts_ema_step(st->ema_m1, m, long_period);
        st->ema_abs_m1 = cxta_ts_ema_step(st->ema_abs_m1, am, long_period);
        st->ema_m2 = cxta_ts_ema_step(st->ema_m2, st->ema_m1, short_period);
        st->ema_abs_m2 = cxta_ts_ema_step(st->ema_abs_m2, st->ema_abs_m1, short_period);
        st->prev_close = close;
        if (st->ema_abs_m2 <= 1e-12) return 0.0;
        return 100.0 * (st->ema_m2 / st->ema_abs_m2);
    }
}

double cxta_tsi(const cxta_series_bar_view* view, int long_period, int short_period) {
    if (!view || !cxta_series_bar_view_valid(view) || view->size < 2) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    cxta_tsi_state st = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    double out = 0.0;
    for (size_t i = 0; i <= idx; ++i) {
        out = cxta_tsi_step(view->bars[i].close, long_period, short_period, &st);
    }
    return out;
}
