/**
 * @file rvi.c
 * @brief Relative Vigor Index helpers.
 */

#include <limits.h>
#include <math.h>

#include <cxta/indicators/rvi.h>
#include <cxta/indicators/macros.h>
#include <cxta/ts/smoothing.h>

static const cxta_scalar_plot_descriptor cxta_rvi_scalar_plot =
    CXTA_SCALAR_PLOT("RVI", "momentum", "#22d3ee", "line", "momentum",
                     "Relative Vigor Index oscillator.",
                     "Use zero-line behavior and turns to compare close/open vigor against range.");

static const cxta_indicator_plot_descriptor cxta_rvi_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("rvi", cxta_rvi_scalar_plot);

static int cxta_rvi_descriptor_period_arg(const double* args,
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

static double cxta_rvi_descriptor_eval(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs) {
    return cxta_rvi(view, cxta_rvi_descriptor_period_arg(args, nargs, 0u, 10));
}

const cxta_indicator_descriptor cxta_rvi_descriptor = {
    "rvi",
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
    cxta_rvi_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_rvi_params,
    CXTA_ARRAY_COUNT(cxta_rvi_params),
    "momentum",
    &cxta_rvi_plot_descriptor,
};

double cxta_rvi(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        if (idx < 3) return 0.0;

        {
            const size_t p = (size_t)cxta_ts_clamp_period(period);
            const size_t available = idx - 2;
            const size_t window = (p < available) ? p : available;
            const size_t start = idx + 1 - window;
            double num = 0.0;
            double den = 0.0;

            for (size_t i = start; i <= idx; ++i) {
                num += view->bars[i].close - view->bars[i].open
                    + 2.0 * (view->bars[i - 1].close - view->bars[i - 1].open)
                    + 2.0 * (view->bars[i - 2].close - view->bars[i - 2].open)
                    + (view->bars[i - 3].close - view->bars[i - 3].open);
                den += view->bars[i].high - view->bars[i].low
                    + 2.0 * (view->bars[i - 1].high - view->bars[i - 1].low)
                    + 2.0 * (view->bars[i - 2].high - view->bars[i - 2].low)
                    + (view->bars[i - 3].high - view->bars[i - 3].low);
            }

            if (den <= 1e-12) return 0.0;
            return num / den;
        }
    }
}
