/**
 * @file volume_ema.c
 * @brief Volume EMA helpers.
 */

#include <cxta/indicators/volume_ema.h>
#include <cxta/ts/smoothing.h>

double cxta_volume_ema_step(double volume, int period, cxta_volume_ema_state* st) {
    if (!st) return 0.0;
    if (st->initialized == 0.0) {
        st->value = volume;
        st->initialized = 1.0;
        return st->value;
    }
    st->value = cxta_ts_ema_step(st->value, volume, period);
    return st->value;
}

double cxta_volume_ema(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const int p = cxta_ts_clamp_period(period);
    double value = view->bars[0].volume;
    for (size_t i = 1; i <= view->index; ++i) {
        value = cxta_ts_ema_step(value, view->bars[i].volume, p);
    }
    return value;
}
