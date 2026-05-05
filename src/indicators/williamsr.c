/**
 * @file williamsr.c
 * @brief Williams %%R helpers.
 */

#include <limits.h>
#include <math.h>

#include <cxta/indicators/williamsr.h>
#include <cxta/indicators/macros.h>
#include <cxta/ts/smoothing.h>

static const cxta_scalar_plot_descriptor cxta_williams_r_scalar_plot =
    CXTA_SCALAR_PLOT("Williams %R", "rsi", "#22d3ee", "line", "rsi",
                     "Williams %R bounded momentum oscillator.",
                     "Use overbought/oversold zones and reversals near range extremes.");

static const cxta_indicator_plot_descriptor cxta_williams_r_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("williams_r", cxta_williams_r_scalar_plot);

static int cxta_williams_r_descriptor_period_arg(const double* args,
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

static double cxta_williams_r_descriptor_eval(const cxta_series_bar_view* view,
                                              const double* args,
                                              size_t nargs) {
    return cxta_williamsr(
        view, cxta_williams_r_descriptor_period_arg(args, nargs, 0u, 14));
}

const cxta_indicator_descriptor cxta_williams_r_descriptor = {
    "williams_r",
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
    cxta_williams_r_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_williams_r_params,
    CXTA_ARRAY_COUNT(cxta_williams_r_params),
    "rsi",
    &cxta_williams_r_plot_descriptor,
};

double cxta_williamsr(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t window = (p < (idx + 1)) ? p : (idx + 1);
    const size_t start = idx + 1 - window;

    double highest = view->bars[start].high;
    double lowest = view->bars[start].low;
    for (size_t i = start + 1; i <= idx; ++i) {
        if (view->bars[i].high > highest) highest = view->bars[i].high;
        if (view->bars[i].low < lowest) lowest = view->bars[i].low;
    }

    {
        const double range = highest - lowest;
        if (range <= 0.0) return 0.0;
        return -100.0 * ((highest - view->bars[idx].close) / range);
    }
}
