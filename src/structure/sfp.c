/**
 * @file sfp.c
 * @brief Swing Failure Pattern (SFP) helpers.
 */

#include <cxta/structure/sfp.h>

static void cxta_struct_sfp_state_reset(cxta_struct_sfp_state* out) {
    out->sfp_high = 0.0;
    out->sfp_low = 0.0;
    out->level = 0.0;
}

int cxta_struct_sfp_state_compute(const cxta_series_bar_view*   view,
                          const cxta_struct_pivot_state* pivot,
                          cxta_struct_sfp_state*         out) {
    if (!view || !pivot || !out) return 0;
    cxta_struct_sfp_state_reset(out);
    if (!cxta_series_bar_view_valid(view)) return 1;

    const cxta_series_bar* bar = cxta_series_bar_view_current(view);
    if (!bar) return 1;

    if (pivot->has_last_high &&
        bar->high > pivot->high &&
        bar->close < pivot->high) {
        out->sfp_high = 1.0;
        out->level = pivot->high;
    }

    if (pivot->has_last_low &&
        bar->low < pivot->low &&
        bar->close > pivot->low) {
        out->sfp_low = 1.0;
        out->level = pivot->low;
    }

    return 1;
}
