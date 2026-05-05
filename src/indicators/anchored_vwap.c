/**
 * @file anchored_vwap.c
 * @brief Anchored VWAP helpers.
 */

#include <cxta/indicators/anchored_vwap.h>
#include <cxta/indicators/macros.h>
#include <cxta/ts/smoothing.h>

static const cxta_scalar_plot_descriptor cxta_anchored_vwap_scalar_plot =
    CXTA_SCALAR_PLOT("Anchored VWAP", "price", "#f59e0b", "line", "price",
                     "VWAP anchored to a fixed lookback.",
                     "Use as event/lookback fair-value reference; price acceptance around it can define bias.");

static const cxta_indicator_plot_descriptor cxta_anchored_vwap_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("anchored_vwap", cxta_anchored_vwap_scalar_plot);

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

static double cxta_anchored_vwap_desc_eval(const cxta_series_bar_view* view,
                                           const double* args,
                                           size_t nargs) {
    int anchor_bars = cxta_descriptor_int_arg(args, nargs, 0u, 0);
    if (anchor_bars > 0) anchor_bars = cxta_ts_clamp_period(anchor_bars);
    return cxta_anchored_vwap(view, anchor_bars);
}

const cxta_indicator_descriptor cxta_anchored_vwap_descriptor = {
    "anchored_vwap",
    0,
    1,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    0u,
    NULL,
    0u,
    cxta_anchored_vwap_desc_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_anchored_vwap_params,
    CXTA_ARRAY_COUNT(cxta_anchored_vwap_params),
    "price",
    &cxta_anchored_vwap_plot_descriptor,
};
