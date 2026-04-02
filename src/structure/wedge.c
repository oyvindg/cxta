/**
 * @file wedge.c
 * @brief Wedge detection over confirmed pivot highs and lows.
 */

#include <cxta/structure/wedge.h>

#include <cxta/structure/pivot.h>
#include <cxta/ts/smoothing.h>
#include <cxta/ts/wedge.h>

#include <math.h>

typedef struct {
    int valid1;
    int valid2;
    size_t x1;
    size_t x2;
    double y1;
    double y2;
} cxta_struct_wedge_anchor;

typedef struct {
    cxta_struct_wedge_anchor highs;
    cxta_struct_wedge_anchor lows;
} cxta_struct_wedge_scan;

typedef struct {
    int valid;
    double score;
    cxta_struct_wedge_state state;
} cxta_struct_wedge_scored;

static double cxta_struct_wedge_nonneg(double value) {
    return (value < 0.0) ? 0.0 : value;
}

static int cxta_struct_wedge_read_int_arg(const double* args,
                                          size_t nargs,
                                          size_t index,
                                          int fallback) {
    const double raw = (!args || index >= nargs) ? (double)fallback : args[index];
    return isfinite(raw) ? (int)llround(raw) : fallback;
}

static double cxta_struct_wedge_read_positive_arg(const double* args,
                                                  size_t nargs,
                                                  size_t index,
                                                  double fallback) {
    const double raw = (!args || index >= nargs) ? fallback : args[index];
    return (isfinite(raw) && raw > 0.0) ? raw : fallback;
}

static void cxta_struct_wedge_seed_defaults(const cxta_series_bar_view* view,
                                            cxta_struct_wedge_state* out) {
    const cxta_series_bar* current;

    if (!out) return;

    out->upper = 0.0;
    out->lower = 0.0;
    out->width = 0.0;
    out->upper_slope = 0.0;
    out->lower_slope = 0.0;
    out->active = 0.0;
    out->rising = 0.0;
    out->falling = 0.0;
    out->breakdown = 0.0;
    out->breakout = 0.0;
    out->has_pivots = 0;

    current = cxta_series_bar_view_current(view);
    if (!current) return;

    out->upper = current->high;
    out->lower = current->low;
    out->width = cxta_struct_wedge_nonneg(out->upper - out->lower);
}

static void cxta_struct_wedge_shift_and_store(cxta_struct_wedge_anchor* anchor,
                                              size_t x,
                                              double y) {
    if (!anchor) return;
    anchor->valid1 = anchor->valid2;
    anchor->x1 = anchor->x2;
    anchor->y1 = anchor->y2;
    anchor->valid2 = 1;
    anchor->x2 = x;
    anchor->y2 = y;
}

static cxta_struct_wedge_scan cxta_struct_wedge_scan_pivots(const cxta_series_bar_view* view,
                                                            size_t first,
                                                            size_t last,
                                                            int left,
                                                            int right) {
    cxta_struct_wedge_scan scan = {0};
    size_t i;

    for (i = first; i <= last; ++i) {
        const cxta_series_bar* bar = cxta_series_bar_view_at(view, i);
        if (!bar) continue;

        if (cxta_struct_pivot_is_high(view, i, left, right)) {
            cxta_struct_wedge_shift_and_store(&scan.highs, i, bar->high);
        }
        if (cxta_struct_pivot_is_low(view, i, left, right)) {
            cxta_struct_wedge_shift_and_store(&scan.lows, i, bar->low);
        }
    }

    return scan;
}

static void cxta_struct_wedge_make_from_scan(const cxta_series_bar_view* view,
                                             size_t idx,
                                             const cxta_struct_wedge_scan* scan,
                                             double max_width_ratio,
                                             cxta_struct_wedge_state* out) {
    cxta_ts_wedge_result wedge;
    const cxta_series_bar* current = cxta_series_bar_view_current(view);

    cxta_struct_wedge_seed_defaults(view, out);
    if (!out || !scan) return;

    if (scan->highs.valid2) out->upper = scan->highs.y2;
    if (scan->lows.valid2) out->lower = scan->lows.y2;
    out->width = cxta_struct_wedge_nonneg(out->upper - out->lower);

    if (!(scan->highs.valid1 && scan->highs.valid2 && scan->lows.valid1 && scan->lows.valid2)) {
        return;
    }

    wedge = cxta_ts_wedge_from_lines(
        (double)scan->highs.x1, scan->highs.y1,
        (double)scan->highs.x2, scan->highs.y2,
        (double)scan->lows.x1, scan->lows.y1,
        (double)scan->lows.x2, scan->lows.y2,
        (double)idx,
        max_width_ratio);

    out->has_pivots = 1;
    out->upper = wedge.upper;
    out->lower = wedge.lower;
    out->width = wedge.width;
    out->upper_slope = wedge.upper_slope;
    out->lower_slope = wedge.lower_slope;
    out->active = wedge.active;
    out->rising = wedge.rising;
    out->falling = wedge.falling;
    out->breakdown = (current && out->rising > 0.0 && current->close < out->lower) ? 1.0 : 0.0;
    out->breakout = (current && out->falling > 0.0 && current->close > out->upper) ? 1.0 : 0.0;
}

static double cxta_struct_wedge_score(const cxta_struct_wedge_scan* scan,
                                      const cxta_struct_wedge_state* out,
                                      size_t idx) {
    size_t last_pivot;
    size_t pivot_age;
    double upper_span;
    double lower_span;
    double min_span;
    double slope_gap;

    if (!scan || !out || !out->has_pivots) return -1.0e100;

    last_pivot = (scan->highs.x2 > scan->lows.x2) ? scan->highs.x2 : scan->lows.x2;
    pivot_age = (idx > last_pivot) ? (idx - last_pivot) : 0u;
    upper_span = (double)(scan->highs.x2 - scan->highs.x1);
    lower_span = (double)(scan->lows.x2 - scan->lows.x1);
    min_span = (upper_span < lower_span) ? upper_span : lower_span;
    slope_gap = fabs(out->lower_slope - out->upper_slope);

    return (out->active > 0.5 ? 1000.0 : 0.0) +
           ((out->breakdown > 0.5 || out->breakout > 0.5) ? 100.0 : 0.0) +
           (20.0 / (1.0 + (double)pivot_age)) +
           min_span +
           (10.0 * slope_gap) -
           out->width;
}

static cxta_struct_wedge_scored cxta_struct_wedge_auto_detect(const cxta_series_bar_view* view,
                                                              size_t first,
                                                              size_t last,
                                                              size_t idx,
                                                              int lookback,
                                                              double max_width_ratio) {
    cxta_struct_wedge_scored best = {0};
    const int max_strength = ((lookback / 4) > 8) ? 8 : ((lookback / 4) < 2 ? 2 : (lookback / 4));
    int left;
    int right;

    for (left = 1; left <= max_strength; ++left) {
        for (right = 1; right <= max_strength; ++right) {
            cxta_struct_wedge_scan scan;
            cxta_struct_wedge_state state = {0};
            const size_t left_n = (size_t)left;
            const size_t right_n = (size_t)right;
            const size_t candidate_first = (first > left_n) ? first : left_n;
            const size_t candidate_last = idx - right_n;
            double score;

            if (idx < (size_t)(left + right + 2)) continue;
            if (candidate_last < candidate_first) continue;

            scan = cxta_struct_wedge_scan_pivots(view, candidate_first, candidate_last, left, right);
            cxta_struct_wedge_make_from_scan(view, idx, &scan, max_width_ratio, &state);
            score = cxta_struct_wedge_score(&scan, &state, idx);
            if (!best.valid || score > best.score) {
                best.valid = 1;
                best.score = score;
                best.state = state;
            }
        }
    }

    return best;
}

int cxta_struct_wedge_args_parse(const double* args,
                                 size_t nargs,
                                 size_t current_index,
                                 cxta_struct_wedge_args* out) {
    const int raw_left = cxta_struct_wedge_read_int_arg(args, nargs, 0u, 2);
    const int raw_right = cxta_struct_wedge_read_int_arg(args, nargs, 1u, 2);
    (void)current_index;

    if (!out) return 0;

    out->auto_left_right = (raw_left <= 0 || raw_right <= 0) ? 1 : 0;
    out->left = cxta_ts_clamp_period(raw_left);
    out->right = cxta_ts_clamp_period(raw_right);
    out->lookback = cxta_ts_clamp_period(cxta_struct_wedge_read_int_arg(args, nargs, 2u, 40));
    out->max_width_ratio = cxta_struct_wedge_read_positive_arg(args, nargs, 3u, 0.85);

    if (out->left < 1) out->left = 1;
    if (out->right < 1) out->right = 1;
    if (out->lookback < 1) out->lookback = 1;
    return 1;
}

int cxta_struct_wedge_state_compute(const cxta_series_bar_view* view,
                                    const cxta_struct_wedge_args* args,
                                    cxta_struct_wedge_state* out) {
    cxta_struct_wedge_args parsed = {0};
    size_t idx;
    size_t start;
    size_t first;
    size_t last;

    if (!out) return 0;
    cxta_struct_wedge_seed_defaults(view, out);
    if (!cxta_series_bar_view_valid(view)) return 1;

    if (args) {
        parsed = *args;
    } else if (!cxta_struct_wedge_args_parse(NULL, 0u, view->index, &parsed)) {
        return 0;
    }

    idx = cxta_series_clamp_index(view->size, view->index);
    if (idx < (size_t)(parsed.left + parsed.right + 2)) return 1;

    start = (idx + 1u > (size_t)parsed.lookback)
        ? (idx + 1u - (size_t)parsed.lookback)
        : 0u;
    first = ((size_t)parsed.left > start) ? (size_t)parsed.left : start;
    last = idx - (size_t)parsed.right;
    if (last < first) return 1;

    if (parsed.auto_left_right) {
        const cxta_struct_wedge_scored best =
            cxta_struct_wedge_auto_detect(view, start, last, idx, parsed.lookback, parsed.max_width_ratio);
        if (best.valid) *out = best.state;
        return 1;
    }

    {
        const cxta_struct_wedge_scan scan =
            cxta_struct_wedge_scan_pivots(view, first, last, parsed.left, parsed.right);
        cxta_struct_wedge_make_from_scan(view, idx, &scan, parsed.max_width_ratio, out);
    }
    return 1;
}

void cxta_struct_wedge_state_mask_geometry(cxta_struct_wedge_state* out) {
    if (!out) return;
    if (out->has_pivots && out->active >= 0.5) return;

    out->upper = NAN;
    out->lower = NAN;
    out->width = NAN;
    out->upper_slope = NAN;
    out->lower_slope = NAN;
}
