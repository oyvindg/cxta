/**
 * @file truerange.c
 * @brief True Range indicator wrapper.
 */

#include <cxta/indicators/truerange.h>
#include <cxta/ts/range.h>

double cxta_truerange(const cxta_series_bar_view* view) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;
    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const cxta_series_bar* cur = &view->bars[idx];
    if (idx == 0) return cur->high - cur->low;
    return cxta_ts_true_range(cur->high, cur->low, view->bars[idx - 1].close);
}
