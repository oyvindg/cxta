/**
 * @file roc.c
 * @brief Rate of Change helpers.
 */

#include <cxta/indicators/roc.h>
#include <cxta/ts/smoothing.h>

double cxta_roc(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t ref = (p <= idx) ? (idx - p) : 0;
    const double ref_close = view->bars[ref].close;
    if (ref_close == 0.0) return 0.0;
    return (view->bars[idx].close / ref_close - 1.0) * 100.0;
}
