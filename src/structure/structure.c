/**
 * @file structure.c
 * @brief Structure classification helpers derived from pivot state.
 */

#include <cxta/structure/structure.h>
#include <math.h>

int cxta_struct_structure_state_compute(const cxta_struct_pivot_state* pivot, cxta_struct_structure_state* out) {
    if (!pivot || !out) return 0;

    out->hh = 0.0;
    out->hl = 0.0;
    out->lh = 0.0;
    out->ll = 0.0;
    out->last_high = pivot->has_last_high ? pivot->high : 0.0;
    out->last_low = pivot->has_last_low ? pivot->low : 0.0;
    out->strength = 0.0;

    int hh_count = 0;
    int hl_count = 0;
    int lh_count = 0;
    int ll_count = 0;

    if (pivot->has_prev_high && pivot->has_last_high) {
        if (pivot->high > pivot->prev_high) {
            ++hh_count;
            if (pivot->is_high != 0.0) out->hh = 1.0;
        } else if (pivot->high < pivot->prev_high) {
            ++lh_count;
            if (pivot->is_high != 0.0) out->lh = 1.0;
        }
    }

    if (pivot->has_prev_low && pivot->has_last_low) {
        if (pivot->low > pivot->prev_low) {
            ++hl_count;
            if (pivot->is_low != 0.0) out->hl = 1.0;
        } else if (pivot->low < pivot->prev_low) {
            ++ll_count;
            if (pivot->is_low != 0.0) out->ll = 1.0;
        }
    }

    const int up_count = hh_count + hl_count;
    const int down_count = lh_count + ll_count;
    const int total = up_count + down_count;
    if (total > 0) {
        out->strength = (double)fabs((double)(up_count - down_count)) / (double)total;
    }
    return 1;
}

