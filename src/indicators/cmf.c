/**
 * @file cmf.c
 * @brief Chaikin Money Flow helpers.
 */

#include <cxta/indicators/cmf.h>
#include <cxta/ts/smoothing.h>

double cxta_cmf(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t window = (p < (idx + 1)) ? p : (idx + 1);
    const size_t start = idx + 1 - window;

    double mfv_sum = 0.0;
    double vol_sum = 0.0;
    for (size_t i = start; i <= idx; ++i) {
        const double hl = view->bars[i].high - view->bars[i].low;
        const double mfm = (hl <= 0.0)
            ? 0.0
            : (((view->bars[i].close - view->bars[i].low) -
                (view->bars[i].high - view->bars[i].close)) / hl);
        mfv_sum += mfm * view->bars[i].volume;
        vol_sum += view->bars[i].volume;
    }

    if (vol_sum <= 0.0) return 0.0;
    return mfv_sum / vol_sum;
}
