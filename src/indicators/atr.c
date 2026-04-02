/**
 * @file atr.c
 * @brief Average True Range helpers.
 */

#include <cxta/indicators/atr.h>
#include <cxta/ts/range.h>
#include <cxta/ts/smoothing.h>

double cxta_atr_step(double high, double low, double close, int period, cxta_atr_state* st) {
    if (!st) return 0.0;
    if (st->initialized == 0.0) {
        st->value = high - low;
        st->prev_close = close;
        st->initialized = 1.0;
        return st->value;
    }

    st->value = cxta_ts_wilder_step(
        st->value,
        cxta_ts_true_range(high, low, st->prev_close),
        period);
    st->prev_close = close;
    return st->value;
}

double cxta_atr(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const int p = cxta_ts_clamp_period(period);
    if (idx == 0) return view->bars[0].high - view->bars[0].low;

    const size_t seed_end = ((size_t)p < idx) ? (size_t)p : idx;
    double atr = 0.0;
    for (size_t i = 1; i <= seed_end; ++i) {
        atr += cxta_ts_true_range(
            view->bars[i].high,
            view->bars[i].low,
            view->bars[i - 1].close);
    }
    atr /= (double)seed_end;

    for (size_t i = seed_end + 1; i <= idx; ++i) {
        atr = cxta_ts_wilder_step(
            atr,
            cxta_ts_true_range(view->bars[i].high, view->bars[i].low, view->bars[i - 1].close),
            p);
    }
    return atr;
}
