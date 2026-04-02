/**
 * @file zigzag.c
 * @brief Causal ZigZag helpers.
 */

#include <cxta/indicators/zigzag.h>

enum {
    CXTA_ZIGZAG_MAX_PIVOTS = 16,
    CXTA_ZIGZAG_MAX_ORDERED_PIVOTS = CXTA_ZIGZAG_MAX_PIVOTS * 2
};

typedef struct {
    double price;
    size_t index;
    int is_high;
} cxta_zigzag_pivot;

typedef enum {
    CXTA_ZIGZAG_PHASE_UNKNOWN = 0,
    CXTA_ZIGZAG_PHASE_UP = 1,
    CXTA_ZIGZAG_PHASE_DOWN = 2,
} cxta_zigzag_phase;

static void cxta_zigzag_push_pivot(cxta_zigzag_pivot* pivots,
                                   int* count,
                                   double price,
                                   size_t index,
                                   int is_high) {
    if (*count > 0) {
        cxta_zigzag_pivot* prev = &pivots[*count - 1];
        if (prev->is_high == is_high) {
            const int replace = is_high ? (price >= prev->price) : (price <= prev->price);
            if (replace) {
                prev->price = price;
                prev->index = index;
                prev->is_high = is_high;
            }
            return;
        }
    }

    if (*count == CXTA_ZIGZAG_MAX_ORDERED_PIVOTS) {
        for (int i = 1; i < *count; ++i) pivots[i - 1] = pivots[i];
        --(*count);
    }

    pivots[*count].price = price;
    pivots[*count].index = index;
    pivots[*count].is_high = is_high;
    ++(*count);
}

cxta_zigzag_output cxta_zigzag(const cxta_series_bar_view* view,
                               double threshold,
                               int n) {
    cxta_zigzag_output out = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    {
        const size_t len = cxta_series_clamp_index(view->size, view->index) + 1;
        cxta_zigzag_pivot pivots[CXTA_ZIGZAG_MAX_ORDERED_PIVOTS];
        double highs[CXTA_ZIGZAG_MAX_PIVOTS];
        double lows[CXTA_ZIGZAG_MAX_PIVOTS];
        int pivot_count = 0;
        int high_count = 0;
        int low_count = 0;

        if (len < 2 || threshold <= 0.0) return out;
        if (n < 0) n = 0;

        {
            double high_extreme = view->bars[0].high;
            double low_extreme = view->bars[0].low;
            size_t high_index = 0;
            size_t low_index = 0;
            cxta_zigzag_phase phase = CXTA_ZIGZAG_PHASE_UNKNOWN;

            for (size_t i = 1; i < len; ++i) {
                const double hi = view->bars[i].high;
                const double lo = view->bars[i].low;
                const double close = view->bars[i].close;

                if (phase == CXTA_ZIGZAG_PHASE_UNKNOWN) {
                    if (hi >= high_extreme) {
                        high_extreme = hi;
                        high_index = i;
                    }
                    if (lo <= low_extreme) {
                        low_extreme = lo;
                        low_index = i;
                    }

                    if (low_extreme > 0.0 &&
                        low_index < i &&
                        close >= low_extreme * (1.0 + threshold)) {
                        cxta_zigzag_push_pivot(pivots, &pivot_count, low_extreme, low_index, 0);
                        phase = CXTA_ZIGZAG_PHASE_UP;
                        high_extreme = hi;
                        high_index = i;
                        continue;
                    }

                    if (high_extreme > 0.0 &&
                        high_index < i &&
                        close <= high_extreme * (1.0 - threshold)) {
                        cxta_zigzag_push_pivot(pivots, &pivot_count, high_extreme, high_index, 1);
                        phase = CXTA_ZIGZAG_PHASE_DOWN;
                        low_extreme = lo;
                        low_index = i;
                        continue;
                    }
                } else if (phase == CXTA_ZIGZAG_PHASE_UP) {
                    if (hi >= high_extreme) {
                        high_extreme = hi;
                        high_index = i;
                    }
                    if (high_extreme > 0.0 &&
                        high_index < i &&
                        close <= high_extreme * (1.0 - threshold)) {
                        cxta_zigzag_push_pivot(pivots, &pivot_count, high_extreme, high_index, 1);
                        phase = CXTA_ZIGZAG_PHASE_DOWN;
                        low_extreme = lo;
                        low_index = i;
                    }
                } else {
                    if (lo <= low_extreme) {
                        low_extreme = lo;
                        low_index = i;
                    }
                    if (low_extreme > 0.0 &&
                        low_index < i &&
                        close >= low_extreme * (1.0 + threshold)) {
                        cxta_zigzag_push_pivot(pivots, &pivot_count, low_extreme, low_index, 0);
                        phase = CXTA_ZIGZAG_PHASE_UP;
                        high_extreme = hi;
                        high_index = i;
                    }
                }
            }

            if (n == 0) {
                out.direction =
                    (phase == CXTA_ZIGZAG_PHASE_UP) ? 1.0 :
                    (phase == CXTA_ZIGZAG_PHASE_DOWN) ? -1.0 : 0.0;
                if (phase == CXTA_ZIGZAG_PHASE_UP) {
                    out.active = high_extreme;
                    out.active_index = (double)high_index;
                } else if (phase == CXTA_ZIGZAG_PHASE_DOWN) {
                    out.active = low_extreme;
                    out.active_index = (double)low_index;
                }
            }

            for (int i = pivot_count - 1; i >= 0; --i) {
                if (pivots[i].is_high) {
                    if (high_count < CXTA_ZIGZAG_MAX_PIVOTS) highs[high_count++] = pivots[i].price;
                } else {
                    if (low_count < CXTA_ZIGZAG_MAX_PIVOTS) lows[low_count++] = pivots[i].price;
                }
            }

            if (n < high_count) out.high = highs[n];
            if (n < low_count) out.low = lows[n];

            if (n < pivot_count) {
                const cxta_zigzag_pivot pivot = pivots[pivot_count - 1 - n];
                out.last = pivot.price;
                out.is_high = pivot.is_high ? 1.0 : 0.0;
                out.line = pivot.price;
                out.pivot_index = (double)pivot.index;
            }
        }
    }
    return out;
}
