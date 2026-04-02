/**
 * @file ulcer.c
 * @brief Ulcer Index helpers.
 */

#include <cxta/indicators/ulcer.h>
#include <cxta/ts/smoothing.h>
#include <math.h>

double cxta_ulcer(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const size_t p = (size_t)cxta_ts_clamp_period(period);
        const size_t window = (p < (idx + 1)) ? p : (idx + 1);
        const size_t start = idx + 1 - window;
        double peak = view->bars[start].close;
        double sum_sq = 0.0;

        for (size_t i = start + 1; i <= idx; ++i) {
            if (view->bars[i].close > peak) peak = view->bars[i].close;
        }
        if (peak <= 0.0) return 0.0;

        for (size_t i = start; i <= idx; ++i) {
            const double dd = 100.0 * (view->bars[i].close - peak) / peak;
            sum_sq += dd * dd;
        }
        return sqrt(sum_sq / (double)window);
    }
}
