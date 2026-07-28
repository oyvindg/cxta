/**
 * @file pivot.c
 * @brief Pivot argument/state helpers over bar views.
 */

#include <cxta/structure/pivot.h>
#include <math.h>

int cxta_struct_pivot_args_parse(const double* args, size_t nargs, size_t current_index, cxta_struct_pivot_args* out) {
    if (!out) return 0;
    const int fallback_lookback = (int)current_index + 1;
    out->left = 1;
    out->right = 1;
    out->lookback = (fallback_lookback < 1) ? 1 : fallback_lookback;
    if (!args || nargs < 2) return 1;

    const int left = (int)llround(args[0]);
    const int right = (int)llround(args[1]);
    out->left = (left < 1) ? 1 : left;
    out->right = (right < 1) ? 1 : right;
    if (nargs >= 3) {
        const int lookback = (int)llround(args[2]);
        out->lookback = (lookback < 1) ? out->lookback : lookback;
    }
    return 1;
}

int cxta_struct_pivot_is_high(const cxta_series_bar_view* view, size_t pivot_index, int left, int right) {
    if (!cxta_series_bar_view_valid(view)) return 0;
    const size_t left_n = (left < 1) ? 1u : (size_t)left;
    const size_t right_n = (right < 1) ? 1u : (size_t)right;
    if (pivot_index < left_n || pivot_index + right_n >= view->size) return 0;

    const cxta_series_bar* pivot_bar = cxta_series_bar_view_at(view, pivot_index);
    if (!pivot_bar) return 0;
    const double pivot = pivot_bar->high;
    const size_t start = pivot_index - left_n;
    const size_t end = pivot_index + right_n;
    for (size_t i = start; i <= end; ++i) {
        if (i == pivot_index) continue;
        const cxta_series_bar* b = cxta_series_bar_view_at(view, i);
        if (!b) return 0;
        if (b->high >= pivot) return 0;
    }
    return 1;
}

int cxta_struct_pivot_is_low(const cxta_series_bar_view* view, size_t pivot_index, int left, int right) {
    if (!cxta_series_bar_view_valid(view)) return 0;
    const size_t left_n = (left < 1) ? 1u : (size_t)left;
    const size_t right_n = (right < 1) ? 1u : (size_t)right;
    if (pivot_index < left_n || pivot_index + right_n >= view->size) return 0;

    const cxta_series_bar* pivot_bar = cxta_series_bar_view_at(view, pivot_index);
    if (!pivot_bar) return 0;
    const double pivot = pivot_bar->low;
    const size_t start = pivot_index - left_n;
    const size_t end = pivot_index + right_n;
    for (size_t i = start; i <= end; ++i) {
        if (i == pivot_index) continue;
        const cxta_series_bar* b = cxta_series_bar_view_at(view, i);
        if (!b) return 0;
        if (b->low <= pivot) return 0;
    }
    return 1;
}

int cxta_struct_pivot_state_compute(const cxta_series_bar_view* view, const cxta_struct_pivot_args* args, cxta_struct_pivot_state* out) {
    if (!out || !view || !args) return 0;
    out->high = 0.0;
    out->low = 0.0;
    out->is_high = 0.0;
    out->is_low = 0.0;
    out->line = 0.0;
    out->pivot_index = 0.0;
    out->prev_high = 0.0;
    out->prev_low = 0.0;
    out->has_prev_high = 0;
    out->has_prev_low = 0;
    out->has_last_high = 0;
    out->has_last_low = 0;
    if (!cxta_series_bar_view_valid(view)) return 1;

    const size_t idx = view->index;
    const size_t lookback = (args->lookback < 1) ? 1u : (size_t)args->lookback;
    const size_t window = (lookback < (idx + 1)) ? lookback : (idx + 1);
    const size_t start = idx + 1 - window;
    const size_t right_n = (args->right < 1) ? 1u : (size_t)args->right;
    if (idx < right_n) return 1;

    const size_t end_confirmed = idx - right_n;
    for (size_t pivot_idx = start; pivot_idx <= end_confirmed; ++pivot_idx) {
        const int high_pivot = cxta_struct_pivot_is_high(view, pivot_idx, args->left, args->right);
        const int low_pivot = cxta_struct_pivot_is_low(view, pivot_idx, args->left, args->right);

        if (high_pivot) {
            if (out->has_last_high) {
                out->prev_high = out->high;
                out->has_prev_high = 1;
            }
            const cxta_series_bar* b = cxta_series_bar_view_at(view, pivot_idx);
            if (!b) continue;
            out->high = b->high;
            out->line = b->high;
            out->pivot_index = (double)pivot_idx;
            out->has_last_high = 1;
            if (pivot_idx == end_confirmed) out->is_high = 1.0;
        }

        if (low_pivot) {
            if (out->has_last_low) {
                out->prev_low = out->low;
                out->has_prev_low = 1;
            }
            const cxta_series_bar* b = cxta_series_bar_view_at(view, pivot_idx);
            if (!b) continue;
            out->low = b->low;
            out->line = b->low;
            out->pivot_index = (double)pivot_idx;
            out->has_last_low = 1;
            if (pivot_idx == end_confirmed) out->is_low = 1.0;
        }
    }
    return 1;
}
