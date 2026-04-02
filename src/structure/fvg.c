/**
 * @file fvg.c
 * @brief Fair Value Gap (FVG) helpers.
 */

#include <cxta/structure/fvg.h>

static void cxta_struct_fvg_state_reset(cxta_struct_fvg_state* out) {
    out->bullish = 0.0;
    out->bearish = 0.0;
    out->top = 0.0;
    out->bottom = 0.0;
    out->filled = 0.0;
}

int cxta_struct_fvg_state_compute(const cxta_series_bar_view* view,
                          size_t              lookback,
                          cxta_struct_fvg_state*      out) {
    if (!view || !out) return 0;
    cxta_struct_fvg_state_reset(out);
    if (!cxta_series_bar_view_valid(view)) return 1;

    const size_t idx = view->index;
    if (idx < 2) return 1;

    size_t window = lookback;
    if (window < 1 || window > (idx + 1)) window = idx + 1;

    size_t start = idx + 1 - window;
    if (start < 2) start = 2;

    for (size_t i = start; i <= idx; ++i) {
        const cxta_series_bar* left = cxta_series_bar_view_at(view, i - 2);
        const cxta_series_bar* right = cxta_series_bar_view_at(view, i);
        if (!left || !right) continue;

        int dir = 0;
        double top = 0.0;
        double bottom = 0.0;

        if (left->high < right->low) {
            dir = 1;
            top = right->low;
            bottom = left->high;
        } else if (left->low > right->high) {
            dir = -1;
            top = left->low;
            bottom = right->high;
        } else {
            continue;
        }

        out->bullish = (dir > 0) ? 1.0 : 0.0;
        out->bearish = (dir < 0) ? 1.0 : 0.0;
        out->top = top;
        out->bottom = bottom;
        out->filled = 0.0;

        for (size_t j = i + 1; j <= idx; ++j) {
            const cxta_series_bar* b = cxta_series_bar_view_at(view, j);
            if (!b) continue;
            if (dir > 0) {
                if (b->low <= bottom) {
                    out->filled = 1.0;
                    break;
                }
            } else {
                if (b->high >= top) {
                    out->filled = 1.0;
                    break;
                }
            }
        }
    }

    return 1;
}
