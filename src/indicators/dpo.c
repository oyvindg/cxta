/**
 * @file dpo.c
 * @brief Detrended Price Oscillator helpers.
 */

#include <cxta/indicators/dpo.h>
#include <cxta/ts/smoothing.h>

double cxta_dpo(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    {
        const int p = cxta_ts_clamp_period(period);
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const size_t shift = (size_t)(p / 2 + 1);
        const size_t ref_idx = (idx > shift) ? (idx - shift) : 0;
        const size_t window = ((size_t)p < (idx + 1)) ? (size_t)p : (idx + 1);
        const size_t start = idx + 1 - window;

        double sum = 0.0;
        for (size_t i = start; i <= idx; ++i) sum += view->bars[i].close;
        return view->bars[ref_idx].close - (sum / (double)window);
    }
}
