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

static double cxta_ease_of_movement_desc_eval(const cxta_series_bar_view* view,
                                              const double* args,
                                              size_t nargs) {
    size_t idx;
    size_t window;
    size_t start;
    size_t i;
    double sum = 0.0;

    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;
    idx = cxta_series_clamp_index(view->size, view->index);
    if (idx == 0u) return 0.0;

    window = (size_t)cxta_descriptor_period_arg(args, nargs, 0u, 14);
    if (window > idx) window = idx;
    start = idx - window + 1u;
    for (i = start; i <= idx; ++i) {
        const cxta_series_bar* cur = &view->bars[i];
        const cxta_series_bar* prev = &view->bars[i - 1u];
        const double mid_move =
            ((cur->high + cur->low) * 0.5) - ((prev->high + prev->low) * 0.5);
        const double box_ratio =
            (cur->volume <= 1e-12) ? 0.0 : ((cur->high - cur->low) / cur->volume);
        sum += mid_move * box_ratio;
    }
    return sum / (double)window;
}

const cxta_indicator_descriptor cxta_ease_of_movement_descriptor = {
    "ease_of_movement",
    1,
    1,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    0u,
    NULL,
    0u,
    cxta_ease_of_movement_desc_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_ease_of_movement_params,
    CXTA_ARRAY_COUNT(cxta_ease_of_movement_params),
};
