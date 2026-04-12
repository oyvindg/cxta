/**
 * @file bop.c
 * @brief Balance of Power helpers.
 */

#include <cxta/indicators/macros.h>
#include <cxta/indicators/bop.h>

double cxta_bop(const cxta_series_bar_view* view) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;
    const cxta_series_bar* b = cxta_series_bar_view_current(view);
    const double range = b->high - b->low;
    if (range == 0.0) return 0.0;
    return (b->close - b->open) / range;
}

CXTA_WRAP_BAR_SCALAR_0(cxta_bop_desc_eval, cxta_bop)

const cxta_bridge_fn_spec cxta_bop_bridge_fn_spec = {
    "bop",
    0u,
    0u,
    NULL,
    0u,
    NULL,
    0u,
    1,
};

const cxta_indicator_descriptor cxta_bop_descriptor = {
    "bop",
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
    cxta_bop_desc_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0u,
};
