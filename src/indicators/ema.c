/**
 * @file ema.c
 * @brief Exponential moving average helpers.
 */

#include <cxta/indicators/ema.h>
#include <cxta/ts/smoothing.h>

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
