/**
 * @file frama.c
 * @brief FRAMA helpers.
 */

#include <cxta/indicators/frama.h>
#include <cxta/ts/smoothing.h>
#include <math.h>

static double cxta_frama_range(const cxta_series_bar_view* view, size_t a, size_t b) {
    double hh = view->bars[a].high;
    double ll = view->bars[a].low;
    for (size_t k = a + 1; k <= b; ++k) {
        if (view->bars[k].high > hh) hh = view->bars[k].high;
        if (view->bars[k].low < ll) ll = view->bars[k].low;
    }
    return hh - ll;
}

static double cxta_frama_clamp(double x, double lo, double hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

double cxta_frama(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    {
        const int p = cxta_ts_clamp_period(period);
        const int half = (p / 2 > 0) ? (p / 2) : 1;
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        double frama = view->bars[0].close;

        for (size_t i = 1; i <= idx; ++i) {
            const size_t start = (i > (size_t)p) ? (i - (size_t)p) : 0u;
            const size_t mid = (start + i) / 2u;
            const double n1 = cxta_frama_range(view, start, mid) / (double)half;
            const double n2 = cxta_frama_range(view, (mid + 1u <= i) ? (mid + 1u) : mid, i) / (double)half;
            const double n3 = cxta_frama_range(view, start, i) / (double)p;
            double d = 1.0;

            if (n1 > 1e-12 && n2 > 1e-12 && n3 > 1e-12) {
                d = (log(n1 + n2) - log(n3)) / log(2.0);
            }
            d = cxta_frama_clamp(d, 1.0, 2.0);

            {
                double alpha = exp(-4.6 * (d - 1.0));
                alpha = cxta_frama_clamp(alpha, 0.01, 1.0);
                frama = alpha * view->bars[i].close + (1.0 - alpha) * frama;
            }
        }
        return frama;
    }
}
