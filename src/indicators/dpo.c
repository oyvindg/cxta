/**
 * @file dpo.c
 * @brief Detrended Price Oscillator helpers.
 */

#include <limits.h>
#include <math.h>

#include <cxta/indicators/dpo.h>
#include <cxta/indicators/macros.h>
#include <cxta/ts/smoothing.h>

static const cxta_scalar_plot_descriptor cxta_dpo_scalar_plot =
    CXTA_SCALAR_PLOT("DPO", "momentum", "#38bdf8", "line", "momentum",
                     "Detrended Price Oscillator.",
                     "Use oscillation around zero to inspect cycle pressure after removing trend.");

static const cxta_indicator_plot_descriptor cxta_dpo_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("dpo", cxta_dpo_scalar_plot);

static int cxta_dpo_descriptor_period_arg(const double* args,
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

static double cxta_dpo_descriptor_eval(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs) {
    return cxta_dpo(view, cxta_dpo_descriptor_period_arg(args, nargs, 0u, 20));
}

const cxta_indicator_descriptor cxta_dpo_descriptor = {
    "dpo",
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
    cxta_dpo_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_dpo_params,
    CXTA_ARRAY_COUNT(cxta_dpo_params),
    "momentum",
    &cxta_dpo_plot_descriptor,
};

double cxta_dpo(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    {
        const int p = cxta_ts_clamp_period(period);
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const size_t shift = (size_t)(p / 2 + 1);
        const size_t ref_idx = (idx > shift) ? (idx - shift) : 0;
        const size_t window = ((size_t)p < (idx + 1)) ? (size_t)p : (idx + 1);
        const size_t start = idx + 1 - window;

        double sum = 0.0;
        for (size_t i = start; i <= idx; ++i) sum += view->bars[i].close;
        return view->bars[ref_idx].close - (sum / (double)window);
    }
}
