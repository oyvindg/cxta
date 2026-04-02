/**
 * @file extrema.c
 * @brief Rolling extrema helpers.
 */

#include <cxta/indicators/extrema.h>
#include <cxta/ts/smoothing.h>

cxta_extrema_output cxta_extrema(const cxta_series_bar_view* view, int period) {
    const cxta_extrema_output zero = {0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return zero;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t window = (p < (idx + 1)) ? p : (idx + 1);
    const size_t start = idx + 1 - window;

    double hi = view->bars[start].high;
    double lo = view->bars[start].low;
    for (size_t i = start + 1; i <= idx; ++i) {
        if (view->bars[i].high > hi) hi = view->bars[i].high;
        if (view->bars[i].low  < lo) lo = view->bars[i].low;
    }
    const cxta_extrema_output out = {hi, lo, (hi + lo) * 0.5};
    return out;
}
