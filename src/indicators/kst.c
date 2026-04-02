/**
 * @file kst.c
 * @brief KST helpers.
 */

#include <cxta/indicators/kst.h>
#include <cxta/ts/smoothing.h>
#include <math.h>

static double cxta_kst_roc(const cxta_series_bar_view* view, size_t idx, int period) {
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    if (idx < p) return 0.0;

    {
        const double prev = view->bars[idx - p].close;
        if (fabs(prev) <= 1e-12) return 0.0;
        return 100.0 * ((view->bars[idx].close - prev) / prev);
    }
}

static double cxta_kst_sma_roc(const cxta_series_bar_view* view, size_t idx, int roc_p, int sma_p) {
    const size_t sp = (size_t)cxta_ts_clamp_period(sma_p);
    const size_t window = (sp < (idx + 1)) ? sp : (idx + 1);
    const size_t start = idx + 1 - window;
    double sum = 0.0;
    for (size_t i = start; i <= idx; ++i) {
        sum += cxta_kst_roc(view, i, roc_p);
    }
    return sum / (double)window;
}

double cxta_kst(const cxta_series_bar_view* view, int p1, int p2, int p3, int p4) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const double rcma1 = cxta_kst_sma_roc(view, idx, p1, 10);
        const double rcma2 = cxta_kst_sma_roc(view, idx, p2, 10);
        const double rcma3 = cxta_kst_sma_roc(view, idx, p3, 10);
        const double rcma4 = cxta_kst_sma_roc(view, idx, p4, 15);
        return rcma1 + 2.0 * rcma2 + 3.0 * rcma3 + 4.0 * rcma4;
    }
}
