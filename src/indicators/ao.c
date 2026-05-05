/**
 * @file ao.c
 * @brief Awesome Oscillator helpers.
 */

#include <cxta/indicators/ao.h>
#include <cxta/indicators/macros.h>
#include <cxta/ts/smoothing.h>

static const cxta_scalar_plot_descriptor cxta_awesome_oscillator_scalar_plot =
    CXTA_SCALAR_PLOT("Awesome Oscillator", "momentum", "#22d3ee", "histogram", "momentum",
                     "Fast minus slow midpoint SMA oscillator.",
                     "Zero-line crosses and histogram expansion indicate momentum shifts.");

static const cxta_indicator_plot_descriptor cxta_awesome_oscillator_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("awesome_oscillator", cxta_awesome_oscillator_scalar_plot);

static double midpoint_sma(const cxta_series_bar* bars, size_t start, size_t end) {
    double sum = 0.0;
    size_t count = end - start + 1;
    for (size_t i = start; i <= end; ++i) {
        sum += (bars[i].high + bars[i].low) * 0.5;
    }
    return (count > 0) ? (sum / (double)count) : 0.0;
}

double cxta_ao(const cxta_series_bar_view* view) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t w5  = (5  < (idx + 1)) ? 5  : (idx + 1);
    const size_t w34 = (34 < (idx + 1)) ? 34 : (idx + 1);

    const double sma5  = midpoint_sma(view->bars, idx + 1 - w5,  idx);
    const double sma34 = midpoint_sma(view->bars, idx + 1 - w34, idx);
    return sma5 - sma34;
}

static double cxta_ao_midpoint_sma(const cxta_series_bar_view* view,
                                   size_t idx,
                                   int period) {
    const size_t window = ((size_t)period < (idx + 1u)) ? (size_t)period : (idx + 1u);
    const size_t start = idx + 1u - window;
    double sum = 0.0;
    size_t i;

    for (i = start; i <= idx; ++i) {
        sum += (view->bars[i].high + view->bars[i].low) * 0.5;
    }
    return (window > 0u) ? (sum / (double)window) : 0.0;
}

static double cxta_awesome_oscillator_desc_eval(const cxta_series_bar_view* view,
                                              const double* args,
                                              size_t nargs) {
    size_t idx;
    int fast;
    int slow;
    int swap_tmp;

    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;
    idx = cxta_series_clamp_index(view->size, view->index);
    fast = cxta_descriptor_period_arg(args, nargs, 0u, 5);
    slow = cxta_descriptor_period_arg(args, nargs, 1u, 34);
    if (fast > slow) {
        swap_tmp = fast;
        fast = slow;
        slow = swap_tmp;
    }
    return cxta_ao_midpoint_sma(view, idx, fast) - cxta_ao_midpoint_sma(view, idx, slow);
}

const cxta_indicator_descriptor cxta_awesome_oscillator_descriptor = {
    "awesome_oscillator",
    2,
    2,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    0u,
    NULL,
    0u,
    cxta_awesome_oscillator_desc_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_awesome_oscillator_params,
    CXTA_ARRAY_COUNT(cxta_awesome_oscillator_params),
    "momentum",
    &cxta_awesome_oscillator_plot_descriptor,
};
