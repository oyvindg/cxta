/**
 * @file wclose.c
 * @brief Weighted close price helpers.
 */

#include <cxta/indicators/wclose.h>

double cxta_wclose(const cxta_series_bar_view* view) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;
    const cxta_series_bar* b = cxta_series_bar_view_current(view);
    return (b->high + b->low + 2.0 * b->close) * 0.25;
}
