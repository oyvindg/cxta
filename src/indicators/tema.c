/**
 * @file tema.c
 * @brief Triple Exponential Moving Average helpers.
 */

#include <cxta/indicators/macros.h>
#include <cxta/indicators/tema.h>
#include <cxta/ts/smoothing.h>

double cxta_tema_step(double close, int period, cxta_tema_state* st) {
    if (!st) return 0.0;
    if (st->initialized == 0.0) {
        st->ema1 = close;
        st->ema2 = close;
        st->ema3 = close;
        st->initialized = 1.0;
        return close;
    }
    st->ema1 = cxta_ts_ema_step(st->ema1, close, period);
    st->ema2 = cxta_ts_ema_step(st->ema2, st->ema1, period);
    st->ema3 = cxta_ts_ema_step(st->ema3, st->ema2, period);
    return 3.0 * st->ema1 - 3.0 * st->ema2 + st->ema3;
}

double cxta_tema(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const int p = cxta_ts_clamp_period(period);
    double ema1 = view->bars[0].close;
    double ema2 = view->bars[0].close;
    double ema3 = view->bars[0].close;
    for (size_t i = 1; i <= view->index; ++i) {
        ema1 = cxta_ts_ema_step(ema1, view->bars[i].close, p);
        ema2 = cxta_ts_ema_step(ema2, ema1, p);
        ema3 = cxta_ts_ema_step(ema3, ema2, p);
    }
    return 3.0 * ema1 - 3.0 * ema2 + ema3;
}

CXTA_WRAP_BAR_SCALAR_1I(cxta_tema_desc_eval_scalar, cxta_tema, 20)

const cxta_indicator_descriptor cxta_tema_descriptor = {
    "tema",
    1,
    1,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    sizeof(cxta_tema_state),
    NULL,
    0u,
    cxta_tema_desc_eval_scalar,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_tema_params,
    CXTA_ARRAY_COUNT(cxta_tema_params),
};
