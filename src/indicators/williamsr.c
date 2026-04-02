/**
 * @file williamsr.c
 * @brief Williams %%R helpers.
 */

#include <cxta/indicators/williamsr.h>
#include <cxta/ts/smoothing.h>

double cxta_williamsr(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t window = (p < (idx + 1)) ? p : (idx + 1);
    const size_t start = idx + 1 - window;

    double highest = view->bars[start].high;
    double lowest = view->bars[start].low;
    for (size_t i = start + 1; i <= idx; ++i) {
        if (view->bars[i].high > highest) highest = view->bars[i].high;
        if (view->bars[i].low < lowest) lowest = view->bars[i].low;
    }

    {
        const double range = highest - lowest;
        if (range <= 0.0) return 0.0;
        return -100.0 * ((highest - view->bars[idx].close) / range);
    }
}
