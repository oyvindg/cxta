/**
 * @file chaikin.c
 * @brief Chaikin Oscillator helpers.
 */

#include <cxta/indicators/chaikin.h>
#include <cxta/ts/smoothing.h>

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
