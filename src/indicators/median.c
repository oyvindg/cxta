/**
 * @file median.c
 * @brief Median price helpers.
 */

#include <cxta/indicators/macros.h>
#include <cxta/indicators/median.h>

static const cxta_scalar_plot_descriptor cxta_median_price_scalar_plot =
    CXTA_SCALAR_PLOT("Median Price", "price", "#bfdbfe", "line", "price",
                     "Median price derived from high and low.",
                     "Use as a price-source transform for overlays and source-aware indicators.");

static const cxta_indicator_plot_descriptor cxta_median_price_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("median_price", cxta_median_price_scalar_plot);

double cxta_median_price(const cxta_series_bar_view* view) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;
    const cxta_series_bar* b = cxta_series_bar_view_current(view);
    return (b->high + b->low) * 0.5;
}

CXTA_WRAP_BAR_SCALAR_0(cxta_median_price_desc_eval, cxta_median_price)

const cxta_bridge_fn_spec cxta_median_price_bridge_fn_spec = {
    "median_price",
    0u,
    0u,
    NULL,
    0u,
    NULL,
    0u,
    1,
};

const cxta_indicator_descriptor cxta_median_price_descriptor = {
    "median_price",
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
    cxta_median_price_desc_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0u,
    "price",
    &cxta_median_price_plot_descriptor,
};
