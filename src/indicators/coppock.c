/**
 * @file coppock.c
 * @brief Coppock Curve helpers.
 */

#include <cxta/indicators/coppock.h>
#include <cxta/indicators/macros.h>
#include <cxta/ts/smoothing.h>

static const cxta_scalar_plot_descriptor cxta_coppock_scalar_plot =
    CXTA_SCALAR_PLOT("Coppock Curve", "momentum", "#22d3ee", "line", "momentum",
                     "Weighted long-horizon ROC momentum curve.",
                     "Use zero-line and slope turns to identify broad momentum shifts.");

static const cxta_indicator_plot_descriptor cxta_coppock_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("coppock_curve", cxta_coppock_scalar_plot);
#include <math.h>

static double cxta_coppock_roc_at(const cxta_series_bar_view* view, size_t idx, int period) {
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    if (idx < p) return 0.0;

    {
        const double prev = view->bars[idx - p].close;
        if (fabs(prev) <= 1e-12) return 0.0;
        return 100.0 * ((view->bars[idx].close - prev) / prev);
    }
}

double cxta_coppock(const cxta_series_bar_view* view,
                    int wma_period,
                    int long_roc,
                    int short_roc) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const size_t w = (size_t)cxta_ts_clamp_period(wma_period);
        const size_t window = (w < (idx + 1)) ? w : (idx + 1);
        const size_t start = idx + 1 - window;
        double weighted_sum = 0.0;
        double weight_sum = 0.0;
        double weight = 1.0;

        for (size_t i = start; i <= idx; ++i, weight += 1.0) {
            const double value =
                cxta_coppock_roc_at(view, i, long_roc) + cxta_coppock_roc_at(view, i, short_roc);
            weighted_sum += value * weight;
            weight_sum += weight;
        }
        return (weight_sum > 0.0) ? (weighted_sum / weight_sum) : 0.0;
    }
}

static double cxta_coppock_curve_desc_eval(const cxta_series_bar_view* view,
                                           const double* args,
                                           size_t nargs) {
    return cxta_coppock(view,
                        cxta_descriptor_period_arg(args, nargs, 0u, 10),
                        cxta_descriptor_period_arg(args, nargs, 1u, 14),
                        cxta_descriptor_period_arg(args, nargs, 2u, 11));
}

const cxta_indicator_descriptor cxta_coppock_curve_descriptor = {
    "coppock_curve",
    0,
    3,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    0u,
    NULL,
    0u,
    cxta_coppock_curve_desc_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_coppock_curve_params,
    CXTA_ARRAY_COUNT(cxta_coppock_curve_params),
    "momentum",
    &cxta_coppock_plot_descriptor,
};
