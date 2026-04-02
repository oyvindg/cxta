/**
 * @file macd.c
 * @brief MACD helpers.
 */

#include <cxta/indicators/macd.h>
#include <cxta/ts/smoothing.h>

cxta_macd_output cxta_macd_step(double close,
                                int fast,
                                int slow,
                                int signal,
                                cxta_macd_state* st) {
    cxta_macd_output out = {0.0, 0.0, 0.0};
    if (!st) return out;

    st->ema_fast = cxta_ts_ema_step(st->ema_fast, close, fast);
    st->ema_slow = cxta_ts_ema_step(st->ema_slow, close, slow);
    out.line = st->ema_fast - st->ema_slow;
    st->signal = cxta_ts_ema_step(st->signal, out.line, signal);
    out.signal = st->signal;
    out.histogram = out.line - out.signal;
    return out;
}

cxta_macd_output cxta_macd(const cxta_series_bar_view* view,
                           int fast,
                           int slow,
                           int signal) {
    cxta_macd_output out = {0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    cxta_macd_state st = {
        view->bars[0].close,
        view->bars[0].close,
        0.0,
    };
    for (size_t i = 1; i <= view->index; ++i) {
        out = cxta_macd_step(view->bars[i].close, fast, slow, signal, &st);
    }
    return out;
}
