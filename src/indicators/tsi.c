/**
 * @file tsi.c
 * @brief True Strength Index helpers.
 */

#include <cxta/indicators/tsi.h>
#include <cxta/ts/smoothing.h>
#include <math.h>

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
