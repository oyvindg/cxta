/**
 * @file kama.c
 * @brief KAMA helpers.
 */

#include <limits.h>
#include <math.h>

#include <cxta/indicators/kama.h>
#include <cxta/indicators/macros.h>
#include <cxta/ts/smoothing.h>

static const cxta_scalar_plot_descriptor cxta_kama_scalar_plot =
    CXTA_SCALAR_PLOT("KAMA", "price", "#0ea5e9", "line", "price",
                     "Kaufman Adaptive Moving Average price overlay.",
                     "Adapts smoothing to efficiency; use slope and distance from price for regime context.");

static const cxta_indicator_plot_descriptor cxta_kama_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("kama", cxta_kama_scalar_plot);

static int cxta_kama_descriptor_period_arg(const double* args,
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

static double cxta_kama_descriptor_eval(const cxta_series_bar_view* view,
                                        const double* args,
                                        size_t nargs) {
    return cxta_kama(view, cxta_kama_descriptor_period_arg(args, nargs, 0u, 20));
}

const cxta_indicator_descriptor cxta_kama_descriptor = {
    "kama",
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
    cxta_kama_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_kama_params,
    CXTA_ARRAY_COUNT(cxta_kama_params),
    "price",
    &cxta_kama_plot_descriptor,
};

double cxta_kama(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    {
        const int p = cxta_ts_clamp_period(period);
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const double fast_sc = 2.0 / 3.0;
        const double slow_sc = 2.0 / 31.0;
        double kama = view->bars[0].close;

        for (size_t i = 1; i <= idx; ++i) {
            const size_t lookback = (i > (size_t)p) ? (i - (size_t)p) : 0u;
            double change = view->bars[i].close - view->bars[lookback].close;
            if (change < 0.0) change = -change;

            {
                double volatility = 0.0;
                for (size_t j = lookback + 1; j <= i; ++j) {
                    double delta = view->bars[j].close - view->bars[j - 1].close;
                    if (delta < 0.0) delta = -delta;
                    volatility += delta;
                }

                {
                    const double er = (volatility <= 1e-12) ? 0.0 : (change / volatility);
                    const double sc = pow(er * (fast_sc - slow_sc) + slow_sc, 2.0);
                    kama = kama + sc * (view->bars[i].close - kama);
                }
            }
        }
        return kama;
    }
}
