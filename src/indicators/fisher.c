/**
 * @file fisher.c
 * @brief Fisher Transform helpers.
 */

#include <cxta/indicators/fisher.h>
#include <cxta/ts/smoothing.h>
#include <math.h>

static double cxta_fisher_clamp(double x, double lo, double hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

double cxta_fisher(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const size_t p = (size_t)cxta_ts_clamp_period(period);
        const size_t window = (p < (idx + 1)) ? p : (idx + 1);
        const size_t start = idx + 1 - window;
        double hh = view->bars[start].high;
        double ll = view->bars[start].low;

        for (size_t i = start + 1; i <= idx; ++i) {
            if (view->bars[i].high > hh) hh = view->bars[i].high;
            if (view->bars[i].low < ll) ll = view->bars[i].low;
        }

        {
            const double denom = hh - ll;
            if (denom <= 1e-12) return 0.0;
            const double x = cxta_fisher_clamp(
                2.0 * ((view->bars[idx].close - ll) / denom) - 1.0,
                -0.999,
                0.999);
            return 0.5 * log((1.0 + x) / (1.0 - x));
        }
    }
}
