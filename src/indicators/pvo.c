/**
 * @file pvo.c
 * @brief Percentage Volume Oscillator helpers.
 */

#include <cxta/indicators/pvo.h>
#include <cxta/ts/smoothing.h>
#include <math.h>

cxta_pvo_output cxta_pvo_step(double volume,
                              int fast,
                              int slow,
                              int signal,
                              cxta_pvo_state* st) {
    cxta_pvo_output out = {0.0, 0.0, 0.0};
    if (!st) return out;
    if (fast > slow) {
        const int tmp = fast;
        fast = slow;
        slow = tmp;
    }

    if (st->initialized == 0.0) {
        st->ema_fast = volume;
        st->ema_slow = volume;
        st->signal = 0.0;
        st->initialized = 1.0;
        return out;
    }

    st->ema_fast = cxta_ts_ema_step(st->ema_fast, volume, fast);
    st->ema_slow = cxta_ts_ema_step(st->ema_slow, volume, slow);
    if (fabs(st->ema_slow) > 1e-12) {
        out.line = 100.0 * ((st->ema_fast - st->ema_slow) / st->ema_slow);
    }
    st->signal = cxta_ts_ema_step(st->signal, out.line, signal);
    out.signal = st->signal;
    out.histogram = out.line - out.signal;
    return out;
}

cxta_pvo_output cxta_pvo(const cxta_series_bar_view* view, int fast, int slow, int signal) {
    cxta_pvo_output out = {0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    cxta_pvo_state st = {0.0, 0.0, 0.0, 0.0};
    for (size_t i = 0; i <= idx; ++i) {
        out = cxta_pvo_step(view->bars[i].volume, fast, slow, signal, &st);
    }
    return out;
}
