/**
 * @file trix.c
 * @brief TRIX helpers.
 */

#include <limits.h>
#include <math.h>

#include <cxta/indicators/trix.h>
#include <cxta/ts/smoothing.h>

static int cxta_trix_descriptor_period_arg(const double* args,
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

static double cxta_trix_descriptor_eval(const cxta_series_bar_view* view,
                                      const double* args,
                                      size_t nargs) {
    return cxta_trix(view, cxta_trix_descriptor_period_arg(args, nargs, 0u, 15));
}

const cxta_indicator_descriptor cxta_trix_descriptor = {
    "trix",
    1,
    1,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    sizeof(cxta_trix_state),
    NULL,
    0u,
    cxta_trix_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_trix_params,
    CXTA_ARRAY_COUNT(cxta_trix_params),
};

double cxta_trix_step(double close, int period, cxta_trix_state* st) {
    if (!st) return 0.0;
    if (st->initialized == 0.0) {
        st->ema1 = close;
        st->ema2 = close;
        st->ema3 = close;
        st->prev_ema3 = close;
        st->initialized = 1.0;
        return 0.0;
    }

    st->prev_ema3 = st->ema3;
    st->ema1 = cxta_ts_ema_step(st->ema1, close, period);
    st->ema2 = cxta_ts_ema_step(st->ema2, st->ema1, period);
    st->ema3 = cxta_ts_ema_step(st->ema3, st->ema2, period);

    if (fabs(st->prev_ema3) <= 1e-12) return 0.0;
    return 100.0 * ((st->ema3 - st->prev_ema3) / st->prev_ema3);
}

double cxta_trix(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view) || view->size < 2) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    cxta_trix_state st = {0.0, 0.0, 0.0, 0.0, 0.0};
    double out = 0.0;
    for (size_t i = 0; i <= idx; ++i) {
        out = cxta_trix_step(view->bars[i].close, period, &st);
    }
    return out;
}
