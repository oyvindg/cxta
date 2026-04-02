/**
 * @file order_block.c
 * @brief Order Block helpers built on top of BOS and pivots.
 */

#include <cxta/structure/order_block.h>
#include <math.h>

static void cxta_struct_order_block_state_reset(cxta_struct_order_block_state* out) {
    out->bullish = 0.0;
    out->bearish = 0.0;
    out->top = 0.0;
    out->bottom = 0.0;
    out->strength = 0.0;
}

static int cxta_struct_is_bullish_bar(const cxta_series_bar* bar) {
    return bar && (bar->close > bar->open);
}

static int cxta_struct_is_bearish_bar(const cxta_series_bar* bar) {
    return bar && (bar->close < bar->open);
}

int cxta_struct_order_block_state_compute(const cxta_series_bar_view*  view,
                                  const cxta_struct_pivot_args* args,
                                  cxta_struct_order_block_state* out) {
    if (!view || !args || !out) return 0;
    cxta_struct_order_block_state_reset(out);
    if (!cxta_series_bar_view_valid(view)) return 1;

    const size_t idx = view->index;
    if (idx < 1) return 1;

    const size_t lookback = (args->lookback < 1) ? (idx + 1) : (size_t)args->lookback;
    const size_t window = (lookback < (idx + 1)) ? lookback : (idx + 1);
    const size_t start = idx + 1 - window;

    for (size_t event_idx = idx + 1; event_idx-- > start;) {
        cxta_series_bar_view event_view = *view;
        event_view.index = event_idx;

        cxta_struct_pivot_state piv = {0};
        cxta_struct_bos_state bos = {0};
        if (!cxta_struct_pivot_state_compute(&event_view, args, &piv)) continue;
        if (!cxta_struct_bos_state_compute(&event_view, &piv, &bos)) continue;

        int dir = 0;
        if (bos.bos_up > 0.5) dir = 1;
        else if (bos.bos_down > 0.5) dir = -1;
        else continue;

        for (size_t j = event_idx; j > start; --j) {
            const size_t candle_idx = j - 1;
            const cxta_series_bar* block_bar = cxta_series_bar_view_at(view, candle_idx);
            if (!block_bar) continue;

            const int match = (dir > 0)
                ? cxta_struct_is_bearish_bar(block_bar)
                : cxta_struct_is_bullish_bar(block_bar);
            if (!match) continue;

            const double top = fmax(block_bar->open, block_bar->close);
            const double bottom = fmin(block_bar->open, block_bar->close);
            const double zone_size = fmax(top - bottom, 1e-12);

            const cxta_series_bar* bos_bar = cxta_series_bar_view_at(view, event_idx);
            if (!bos_bar) continue;
            const double displacement = fabs(bos_bar->close - bos.level);
            double strength = displacement / zone_size;
            if (strength > 1.0) strength = 1.0;

            out->bullish = (dir > 0) ? 1.0 : 0.0;
            out->bearish = (dir < 0) ? 1.0 : 0.0;
            out->top = top;
            out->bottom = bottom;
            out->strength = strength;
            return 1;
        }
    }

    return 1;
}
