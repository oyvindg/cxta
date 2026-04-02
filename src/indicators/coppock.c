/**
 * @file coppock.c
 * @brief Coppock Curve helpers.
 */

#include <cxta/indicators/coppock.h>
#include <cxta/ts/smoothing.h>
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
