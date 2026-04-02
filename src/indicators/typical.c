/**
 * @file typical.c
 * @brief Typical price helpers.
 */

#include <cxta/indicators/typical.h>

double cxta_typical(const cxta_series_bar_view* view) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;
    const cxta_series_bar* b = cxta_series_bar_view_current(view);
    return cxta_series_typical_price(b);
}
