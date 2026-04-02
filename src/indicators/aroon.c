/**
 * @file aroon.c
 * @brief Aroon helpers.
 */

#include <cxta/indicators/aroon.h>
#include <cxta/ts/smoothing.h>

cxta_aroon_output cxta_aroon(const cxta_series_bar_view* view, int period) {
    cxta_aroon_output out = {0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t lookback = (p < idx) ? p : idx;
    const size_t start = idx - lookback;

    size_t hi_idx = start;
    size_t lo_idx = start;
    double hi = view->bars[start].high;
    double lo = view->bars[start].low;
    for (size_t i = start + 1; i <= idx; ++i) {
        if (view->bars[i].high >= hi) {
            hi = view->bars[i].high;
            hi_idx = i;
        }
        if (view->bars[i].low <= lo) {
            lo = view->bars[i].low;
            lo_idx = i;
        }
    }

    {
        const double denom = (lookback > 0) ? (double)lookback : 1.0;
        const double bars_since_high = (double)(idx - hi_idx);
        const double bars_since_low = (double)(idx - lo_idx);
        out.up = 100.0 * (denom - bars_since_high) / denom;
        out.down = 100.0 * (denom - bars_since_low) / denom;
        out.oscillator = out.up - out.down;
    }
    return out;
}
