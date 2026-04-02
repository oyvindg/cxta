/**
 * @file wma.c
 * @brief Weighted moving average helpers.
 */

#include <cxta/indicators/wma.h>
#include <cxta/ts/smoothing.h>

double cxta_wma(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = view->index;
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t window = (p < (idx + 1)) ? p : (idx + 1);
    const size_t start = idx + 1 - window;

    double weighted_sum = 0.0;
    double weight_sum = 0.0;
    double weight = 1.0;
    for (size_t i = start; i <= idx; ++i, weight += 1.0) {
        weighted_sum += view->bars[i].close * weight;
        weight_sum += weight;
    }
    return (weight_sum > 0.0) ? (weighted_sum / weight_sum) : 0.0;
}
