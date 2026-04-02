/**
 * @file anchored_vwap.c
 * @brief Anchored VWAP helpers.
 */

#include <cxta/indicators/anchored_vwap.h>
#include <cxta/ts/smoothing.h>

double cxta_anchored_vwap(const cxta_series_bar_view* view, int anchor_bars) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const size_t start = (anchor_bars <= 0)
            ? cxta_series_session_start_index(view->bars, view->size, idx)
            : (idx + 1 > (size_t)cxta_ts_clamp_period(anchor_bars)
                ? idx + 1 - (size_t)cxta_ts_clamp_period(anchor_bars)
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
