/**
 * @file donchian.c
 * @brief Donchian channel helpers.
 */

#include <cxta/indicators/donchian.h>
#include <cxta/ts/smoothing.h>

cxta_channel_output cxta_donchian(const cxta_series_bar_view* view, int period) {
    cxta_channel_output out = {0.0, 0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t window = (p < (idx + 1)) ? p : (idx + 1);
    const size_t start = idx + 1 - window;

    out.upper = view->bars[start].high;
    out.lower = view->bars[start].low;
    for (size_t i = start + 1; i <= idx; ++i) {
        if (view->bars[i].high > out.upper) out.upper = view->bars[i].high;
        if (view->bars[i].low < out.lower) out.lower = view->bars[i].low;
    }
    out.middle = (out.upper + out.lower) * 0.5;
    out.width = out.upper - out.lower;
    return out;
}
