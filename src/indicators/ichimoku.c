/**
 * @file ichimoku.c
 * @brief Ichimoku Cloud helpers.
 */

#include <cxta/indicators/ichimoku.h>
#include <cxta/ts/smoothing.h>

static double cxta_ichimoku_midpoint_window(const cxta_series_bar_view* view,
                                            size_t idx,
                                            int period) {
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t window = (p < (idx + 1)) ? p : (idx + 1);
    const size_t start = idx + 1 - window;
    double highest = view->bars[start].high;
    double lowest = view->bars[start].low;

    for (size_t i = start + 1; i <= idx; ++i) {
        if (view->bars[i].high > highest) highest = view->bars[i].high;
        if (view->bars[i].low < lowest) lowest = view->bars[i].low;
    }
    return (highest + lowest) * 0.5;
}

cxta_ichimoku_output cxta_ichimoku(const cxta_series_bar_view* view,
                                   int tenkan_period,
                                   int kijun_period,
                                   int senkou_b_period) {
    cxta_ichimoku_output out = {0.0, 0.0, 0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    if (tenkan_period > kijun_period) {
        const int tmp = tenkan_period;
        tenkan_period = kijun_period;
        kijun_period = tmp;
    }
    if (senkou_b_period <= kijun_period) senkou_b_period = kijun_period + 1;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        out.tenkan = cxta_ichimoku_midpoint_window(view, idx, tenkan_period);
        out.kijun = cxta_ichimoku_midpoint_window(view, idx, kijun_period);
        out.senkou_a = (out.tenkan + out.kijun) * 0.5;
        out.senkou_b = cxta_ichimoku_midpoint_window(view, idx, senkou_b_period);
        out.chikou = view->bars[idx].close;
    }
    return out;
}
