/**
 * @file median.c
 * @brief Median price helpers.
 */

#include <cxta/indicators/median.h>

double cxta_median_price(const cxta_series_bar_view* view) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;
    const cxta_series_bar* b = cxta_series_bar_view_current(view);
    return (b->high + b->low) * 0.5;
}
