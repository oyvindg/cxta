/**
 * @file eom.c
 * @brief Ease of Movement helpers.
 */

#include <cxta/indicators/eom.h>

double cxta_eom(const cxta_series_bar_view* view) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    if (idx == 0) return 0.0;

    const cxta_series_bar* cur  = &view->bars[idx];
    const cxta_series_bar* prev = &view->bars[idx - 1];

    const double range = cur->high - cur->low;
    if (cur->volume == 0.0 || range == 0.0) return 0.0;

    const double midpoint_move = (cur->high + cur->low) * 0.5
                                - (prev->high + prev->low) * 0.5;
    return (midpoint_move * range) / cur->volume;
}
