/**
 * @file frama.c
 * @brief FRAMA helpers.
 */

#include <limits.h>
#include <math.h>

#include <cxta/indicators/frama.h>
#include <cxta/indicators/macros.h>
#include <cxta/ts/smoothing.h>

static const cxta_scalar_plot_descriptor cxta_frama_scalar_plot =
    CXTA_SCALAR_PLOT("FRAMA", "price", "#0284c7", "line", "price",
                     "Fractal Adaptive Moving Average price overlay.",
                     "Adapts to trend/range structure; use slope and price distance as trend context.");

static const cxta_indicator_plot_descriptor cxta_frama_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("frama", cxta_frama_scalar_plot);

static int cxta_frama_descriptor_period_arg(const double* args,
                                            size_t nargs,
                                            size_t index,
                                            int fallback) {
    double raw;

    if (!args || index >= nargs) return fallback;
    raw = args[index];
    if (!isfinite(raw)) return fallback;
    if (raw >= (double)INT_MAX) return INT_MAX;
    if (raw <= (double)INT_MIN) return INT_MIN;
    return cxta_ts_clamp_period((int)llround(raw));
}

static double cxta_frama_descriptor_eval(const cxta_series_bar_view* view,
                                         const double* args,
                                         size_t nargs) {
    return cxta_frama(view, cxta_frama_descriptor_period_arg(args, nargs, 0u, 16));
}

const cxta_indicator_descriptor cxta_frama_descriptor = {
    "frama",
    1,
    1,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    0u,
    NULL,
    0u,
    cxta_frama_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_frama_params,
    CXTA_ARRAY_COUNT(cxta_frama_params),
    "price",
    &cxta_frama_plot_descriptor,
};

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
