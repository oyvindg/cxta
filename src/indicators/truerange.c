/**
 * @file truerange.c
 * @brief True Range indicator wrapper.
 */

#include <cxta/indicators/macros.h>
#include <cxta/indicators/truerange.h>
#include <cxta/ts/range.h>

static const cxta_scalar_plot_descriptor cxta_true_range_scalar_plot =
    CXTA_SCALAR_PLOT("True Range", "volatility", "#fb7185", "line", "volatility",
                     "Single-bar true range volatility measurement.",
                     "Use as raw range input; ATR smooths this series for risk sizing.");

static const cxta_indicator_plot_descriptor cxta_true_range_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("true_range", cxta_true_range_scalar_plot);

double cxta_truerange(const cxta_series_bar_view* view) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;
    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const cxta_series_bar* cur = &view->bars[idx];
    if (idx == 0) return cur->high - cur->low;
    return cxta_ts_true_range(cur->high, cur->low, view->bars[idx - 1].close);
}

CXTA_WRAP_BAR_SCALAR_0(cxta_true_range_desc_eval, cxta_truerange)

const cxta_bridge_fn_spec cxta_true_range_bridge_fn_spec = {
    "true_range",
    0u,
    0u,
    NULL,
    0u,
    NULL,
    0u,
    1,
};

const cxta_indicator_descriptor cxta_true_range_descriptor = {
    "true_range",
    0,
    0,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    0u,
    NULL,
    0u,
    cxta_true_range_desc_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0u,
    "volatility",
    &cxta_true_range_plot_descriptor,
};
