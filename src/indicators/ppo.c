/**
 * @file ppo.c
 * @brief Percentage Price Oscillator helpers.
 */

#include <cxta/indicators/ppo.h>
#include <cxta/ts/smoothing.h>
#include <math.h>

double cxta_ppo_step(double close, int fast, int slow, cxta_ppo_state* st) {
    if (!st) return 0.0;
    if (fast > slow) {
        const int tmp = fast;
        fast = slow;
        slow = tmp;
    }

    if (st->initialized == 0.0) {
        st->ema_fast = close;
        st->ema_slow = close;
        st->initialized = 1.0;
        return 0.0;
    }

    st->ema_fast = cxta_ts_ema_step(st->ema_fast, close, fast);
    st->ema_slow = cxta_ts_ema_step(st->ema_slow, close, slow);
    if (fabs(st->ema_slow) <= 1e-12) return 0.0;
    return 100.0 * ((st->ema_fast - st->ema_slow) / st->ema_slow);
}

double cxta_ppo(const cxta_series_bar_view* view, int fast, int slow) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    cxta_ppo_state st = {0.0, 0.0, 0.0};
    double out = 0.0;
    for (size_t i = 0; i <= idx; ++i) {
        out = cxta_ppo_step(view->bars[i].close, fast, slow, &st);
    }
    return out;
}
