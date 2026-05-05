/**
 * @file dema.c
 * @brief Double Exponential Moving Average helpers.
 */

#include <cxta/indicators/macros.h>
#include <cxta/indicators/dema.h>
#include <cxta/ts/smoothing.h>

static const cxta_scalar_plot_descriptor cxta_dema_scalar_plot =
    CXTA_SCALAR_PLOT("DEMA", "price", "#2dd4bf", "line", "price",
                     "Double Exponential Moving Average price overlay.",
                     "A lower-lag moving average; use slope and crosses for faster trend response.");

static const cxta_indicator_plot_descriptor cxta_dema_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("dema", cxta_dema_scalar_plot);

double cxta_dema_step(double close, int period, cxta_dema_state* st) {
    if (!st) return 0.0;
    if (st->initialized == 0.0) {
        st->ema1 = close;
        st->ema2 = close;
        st->initialized = 1.0;
        return close;
    }
    st->ema1 = cxta_ts_ema_step(st->ema1, close, period);
    st->ema2 = cxta_ts_ema_step(st->ema2, st->ema1, period);
    return 2.0 * st->ema1 - st->ema2;
}

double cxta_dema(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const int p = cxta_ts_clamp_period(period);
    double ema1 = view->bars[0].close;
    double ema2 = view->bars[0].close;
    for (size_t i = 1; i <= view->index; ++i) {
        ema1 = cxta_ts_ema_step(ema1, view->bars[i].close, p);
        ema2 = cxta_ts_ema_step(ema2, ema1, p);
    }
    return 2.0 * ema1 - ema2;
}

CXTA_WRAP_BAR_SCALAR_1I(cxta_dema_desc_eval_scalar, cxta_dema, 20)

const cxta_indicator_descriptor cxta_dema_descriptor = {
    "dema",
    1,
    1,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    sizeof(cxta_dema_state),
    NULL,
    0u,
    cxta_dema_desc_eval_scalar,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_dema_params,
    CXTA_ARRAY_COUNT(cxta_dema_params),
    "price",
    &cxta_dema_plot_descriptor,
};
