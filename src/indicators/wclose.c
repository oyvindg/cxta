/**
 * @file wclose.c
 * @brief Weighted close price helpers.
 */

#include <cxta/indicators/macros.h>
#include <cxta/indicators/wclose.h>

static const cxta_scalar_plot_descriptor cxta_weighted_close_scalar_plot =
    CXTA_SCALAR_PLOT("Weighted Close", "price", "#dbeafe", "line", "price",
                     "Weighted close price derived from high, low, and close.",
                     "Use as a close-weighted source transform for overlays and filters.");

static const cxta_indicator_plot_descriptor cxta_weighted_close_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("weighted_close", cxta_weighted_close_scalar_plot);

double cxta_wclose(const cxta_series_bar_view* view) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;
    const cxta_series_bar* b = cxta_series_bar_view_current(view);
    return (b->high + b->low + 2.0 * b->close) * 0.25;
}

CXTA_WRAP_BAR_SCALAR_0(cxta_weighted_close_desc_eval, cxta_wclose)

const cxta_bridge_fn_spec cxta_weighted_close_bridge_fn_spec = {
    "weighted_close",
    0u,
    0u,
    NULL,
    0u,
    NULL,
    0u,
    1,
};

const cxta_indicator_descriptor cxta_weighted_close_descriptor = {
    "weighted_close",
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
    cxta_weighted_close_desc_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0u,
    "price",
    &cxta_weighted_close_plot_descriptor,
};
