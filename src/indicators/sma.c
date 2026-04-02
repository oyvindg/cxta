/**
 * @file sma.c
 * @brief Simple moving average helpers.
 */

#include <cxta/indicators/sma.h>
#include <cxta/ts/smoothing.h>

double cxta_sma_from_sum(double sum, size_t window) {
    if (window == 0) return 0.0;
    return sum / (double)window;
}

double cxta_sma(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t window = (p < (idx + 1)) ? p : (idx + 1);
    const size_t start = idx + 1 - window;

    double sum = 0.0;
    for (size_t i = start; i <= idx; ++i) {
        sum += view->bars[i].close;
    }
    return cxta_sma_from_sum(sum, window);
}
