/**
 * @file typical.c
 * @brief Typical price helpers.
 */

#include <cxta/indicators/macros.h>
#include <cxta/indicators/typical.h>

double cxta_typical(const cxta_series_bar_view* view) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;
    const cxta_series_bar* b = cxta_series_bar_view_current(view);
    return cxta_series_typical_price(b);
}

CXTA_WRAP_BAR_SCALAR_0(cxta_typical_price_desc_eval, cxta_typical)

const cxta_bridge_fn_spec cxta_typical_price_bridge_fn_spec = {
    "typical_price",
    0u,
    0u,
    NULL,
    0u,
    NULL,
    0u,
    1,
};

const cxta_indicator_descriptor cxta_typical_price_descriptor = {
    "typical_price",
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
    cxta_typical_price_desc_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0u,
};
