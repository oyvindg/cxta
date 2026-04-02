/**
 * @file vwap.c
 * @brief VWAP helpers.
 */

#include <cxta/indicators/vwap.h>
#include <cxta/ts/smoothing.h>

double cxta_vwap_step(double typical_price, double volume, cxta_vwap_state* st) {
    if (!st) return 0.0;
    st->pv_sum += typical_price * volume;
    st->v_sum += volume;
    return (st->v_sum <= 1e-12) ? 0.0 : (st->pv_sum / st->v_sum);
}

double cxta_vwap(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const size_t start = (period <= 0)
            ? 0u
            : (idx + 1 > (size_t)cxta_ts_clamp_period(period)
                ? idx + 1 - (size_t)cxta_ts_clamp_period(period)
                : 0u);
        double pv_sum = 0.0;
        double v_sum = 0.0;

        for (size_t i = start; i <= idx; ++i) {
            const double tp = cxta_series_typical_price(&view->bars[i]);
            pv_sum += tp * view->bars[i].volume;
            v_sum += view->bars[i].volume;
        }
        return (v_sum <= 1e-12) ? 0.0 : (pv_sum / v_sum);
    }
}
