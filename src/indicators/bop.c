/**
 * @file bop.c
 * @brief Balance of Power helpers.
 */

#include <cxta/indicators/bop.h>

double cxta_bop(const cxta_series_bar_view* view) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;
    const cxta_series_bar* b = cxta_series_bar_view_current(view);
    const double range = b->high - b->low;
    if (range == 0.0) return 0.0;
    return (b->close - b->open) / range;
}
