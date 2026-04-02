/**
 * @file bos.c
 * @brief Break-of-structure and change-of-character helpers.
 */

#include <cxta/structure/bos.h>

int cxta_struct_bos_state_compute(const cxta_series_bar_view*   view,
                           const cxta_struct_pivot_state* pivot,
                           cxta_struct_bos_state*         out) {
    if (!view || !pivot || !out) return 0;

    out->bos_up    = 0.0;
    out->bos_down  = 0.0;
    out->level     = 0.0;
    out->choch_up  = 0.0;
    out->choch_down = 0.0;

    if (!cxta_series_bar_view_valid(view)) return 1;

    const cxta_series_bar* bar = cxta_series_bar_view_current(view);
    if (!bar) return 1;
    const double close = bar->close;

    /* Determine prevailing trend from the last two confirmed pivot highs/lows.
     * Bullish trend: HH (last_high > prev_high) AND HL (last_low > prev_low).
     * Bearish trend: LH (last_high < prev_high) AND LL (last_low < prev_low).
     * Mixed or incomplete history: no CHoCH classification. */
    int bullish_trend = 0;
    int bearish_trend = 0;
    if (pivot->has_prev_high && pivot->has_last_high &&
        pivot->has_prev_low  && pivot->has_last_low) {
        const int hh = pivot->high > pivot->prev_high;
        const int hl = pivot->low  > pivot->prev_low;
        const int lh = pivot->high < pivot->prev_high;
        const int ll = pivot->low  < pivot->prev_low;
        bullish_trend = hh & hl;
        bearish_trend = lh & ll;
    }

    /* BOS up: close exceeded the last confirmed pivot high. */
    if (pivot->has_last_high && close > pivot->high) {
        out->bos_up = 1.0;
        out->level  = pivot->high;
        if (bearish_trend) out->choch_up = 1.0;
    }

    /* BOS down: close fell below the last confirmed pivot low. */
    if (pivot->has_last_low && close < pivot->low) {
        out->bos_down = 1.0;
        out->level    = pivot->low;
        if (bullish_trend) out->choch_down = 1.0;
    }

    return 1;
}
