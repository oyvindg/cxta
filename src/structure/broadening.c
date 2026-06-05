/**
 * @file broadening.c
 * @brief Broadening channel detection over confirmed pivot highs and lows.
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
} cxta_struct_broadening_anchor;

typedef struct {
    size_t x;
    double y;
} cxta_struct_broadening_pivot;

typedef struct {
    cxta_struct_broadening_anchor highs;
    cxta_struct_broadening_anchor lows;
    cxta_struct_broadening_pivot high_points[24];
    cxta_struct_broadening_pivot low_points[24];
    size_t high_count;
    size_t low_count;
} cxta_struct_broadening_scan;

typedef struct {
    int valid;
    double score;
    cxta_struct_broadening_state state;
} cxta_struct_broadening_scored;

#define CXTA_STRUCT_BROADENING_MAX_WIDTH_TO_PRICE 0.25
#define CXTA_STRUCT_BROADENING_MAX_SINGLE_EDGE_GAP 0.22
#define CXTA_STRUCT_BROADENING_MAX_COMBINED_EDGE_GAP 0.32

static double cxta_struct_broadening_nonneg(double value) {
    return (value < 0.0) ? 0.0 : value;
}

static int cxta_struct_broadening_read_int_arg(const double* args,
                                               size_t nargs,
                                               size_t index,
                                               int fallback) {
    const double raw = (!args || index >= nargs) ? (double)fallback : args[index];
    return isfinite(raw) ? (int)llround(raw) : fallback;
}

static double cxta_struct_broadening_read_positive_arg(const double* args,
                                                       size_t nargs,
                                                       size_t index,
                                                       double fallback) {
    const double raw = (!args || index >= nargs) ? fallback : args[index];
    return (isfinite(raw) && raw > 0.0) ? raw : fallback;
}

static void cxta_struct_broadening_seed_defaults(const cxta_series_bar_view* view,
                                                 cxta_struct_broadening_state* out) {
    const cxta_series_bar* current;

    if (!out) return;

    out->resistance = 0.0;
    out->support = 0.0;
    out->active_resistance = 0.0;
    out->active_support = 0.0;
    out->resistance_pivot = 0.0;
    out->resistance_pivot_index = 0.0;
    out->support_pivot = 0.0;
    out->support_pivot_index = 0.0;
    out->active_index = 0.0;
    out->width = 0.0;
    out->resistance_slope = 0.0;
    out->support_slope = 0.0;
    out->active = 0.0;
    out->rising = 0.0;
    out->falling = 0.0;
    out->breakout = 0.0;
    out->breakdown = 0.0;
    out->direction = 0.0;
    out->strength = 0.0;
    out->has_pivots = 0;

    current = cxta_series_bar_view_current(view);
    if (!current) return;

    out->resistance = current->high;
    out->support = current->low;
    out->width = cxta_struct_broadening_nonneg(out->resistance - out->support);
    out->active_resistance = NAN;
    out->active_support = NAN;
}

static void cxta_struct_broadening_shift_and_store(cxta_struct_broadening_anchor* anchor,
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

static void cxta_struct_broadening_append_pivot(cxta_struct_broadening_pivot* points,
                                                size_t* count,
                                                size_t capacity,
                                                size_t x,
                                                double y) {
    size_t i;

    if (!points || !count || capacity == 0u) return;
    if (*count < capacity) {
        points[*count].x = x;
        points[*count].y = y;
        *count += 1u;
        return;
    }

    for (i = 1u; i < capacity; ++i) {
        points[i - 1u] = points[i];
    }
    points[capacity - 1u].x = x;
    points[capacity - 1u].y = y;
}

static cxta_struct_broadening_scan cxta_struct_broadening_scan_pivots(
    const cxta_series_bar_view* view,
    size_t first,
    size_t last,
    int left,
    int right) {
    cxta_struct_broadening_scan scan = {0};
    size_t i;

    for (i = first; i <= last; ++i) {
        const cxta_series_bar* bar = cxta_series_bar_view_at(view, i);
        if (!bar) continue;

        if (cxta_struct_pivot_is_high(view, i, left, right)) {
            cxta_struct_broadening_shift_and_store(&scan.highs, i, bar->high);
            cxta_struct_broadening_append_pivot(
                scan.high_points,
                &scan.high_count,
                sizeof(scan.high_points) / sizeof(scan.high_points[0]),
                i,
                bar->high);
        }
        if (cxta_struct_pivot_is_low(view, i, left, right)) {
            cxta_struct_broadening_shift_and_store(&scan.lows, i, bar->low);
            cxta_struct_broadening_append_pivot(
                scan.low_points,
                &scan.low_count,
                sizeof(scan.low_points) / sizeof(scan.low_points[0]),
                i,
                bar->low);
        }
    }

    return scan;
}

static cxta_struct_broadening_scan cxta_struct_broadening_scan_from_pairs(
    const cxta_struct_broadening_scan* source,
    size_t high1,
    size_t high2,
    size_t low1,
    size_t low2) {
    cxta_struct_broadening_scan out = {0};

    if (!source) return out;

    out.highs.valid1 = 1;
    out.highs.valid2 = 1;
    out.highs.x1 = source->high_points[high1].x;
    out.highs.y1 = source->high_points[high1].y;
    out.highs.x2 = source->high_points[high2].x;
    out.highs.y2 = source->high_points[high2].y;

    out.lows.valid1 = 1;
    out.lows.valid2 = 1;
    out.lows.x1 = source->low_points[low1].x;
    out.lows.y1 = source->low_points[low1].y;
    out.lows.x2 = source->low_points[low2].x;
    out.lows.y2 = source->low_points[low2].y;

    return out;
}

static double cxta_struct_broadening_breakout_source_value(const cxta_series_bar* current,
                                                           int breakout_source,
                                                           double trigger_value,
                                                           int has_trigger_value) {
    if (has_trigger_value) return trigger_value;
    if (!current) return NAN;
    return breakout_source == 1 ? current->open : current->close;
}

static double cxta_struct_broadening_current_edge_gap(const cxta_series_bar* current,
                                                      const cxta_struct_broadening_state* out) {
    double upper_gap;
    double lower_gap;

    if (!current || !out || !(out->width > 0.0)) return 1.0;

    upper_gap = fabs(out->resistance - current->high);
    lower_gap = fabs(current->low - out->support);

    return (upper_gap + lower_gap) / out->width;
}

static int cxta_struct_broadening_current_edges_are_tight(
    const cxta_series_bar* current,
    const cxta_struct_broadening_state* out) {
    double upper_gap;
    double lower_gap;

    if (!current || !out || !(out->width > 0.0)) return 0;

    upper_gap = fabs(out->resistance - current->high);
    lower_gap = fabs(current->low - out->support);

    upper_gap /= out->width;
    lower_gap /= out->width;

    if (upper_gap > CXTA_STRUCT_BROADENING_MAX_SINGLE_EDGE_GAP) return 0;
    if (lower_gap > CXTA_STRUCT_BROADENING_MAX_SINGLE_EDGE_GAP) return 0;
    return (upper_gap + lower_gap) <= CXTA_STRUCT_BROADENING_MAX_COMBINED_EDGE_GAP;
}

static void cxta_struct_broadening_make_from_scan(const cxta_series_bar_view* view,
                                                  size_t idx,
                                                  const cxta_struct_broadening_scan* scan,
                                                  double min_width_ratio,
                                                  int breakout_source,
                                                  double trigger_value_arg,
                                                  int has_trigger_value,
                                                  cxta_struct_broadening_state* out) {
    cxta_ts_broadening_result broadening;
    const cxta_series_bar* current = cxta_series_bar_view_current(view);
    const double trigger_value =
        cxta_struct_broadening_breakout_source_value(
            current, breakout_source, trigger_value_arg, has_trigger_value);

    cxta_struct_broadening_seed_defaults(view, out);
    if (!out || !scan) return;

    if (scan->highs.valid2) out->resistance = scan->highs.y2;
    if (scan->lows.valid2) out->support = scan->lows.y2;
    out->width = cxta_struct_broadening_nonneg(out->resistance - out->support);

    if (!(scan->highs.valid1 && scan->highs.valid2 && scan->lows.valid1 && scan->lows.valid2)) {
        return;
    }

    broadening = cxta_ts_broadening_from_lines(
        (double)scan->highs.x1, scan->highs.y1,
        (double)scan->highs.x2, scan->highs.y2,
        (double)scan->lows.x1, scan->lows.y1,
        (double)scan->lows.x2, scan->lows.y2,
        (double)idx,
        min_width_ratio);

    out->has_pivots = 1;
    out->resistance = broadening.resistance;
    out->support = broadening.support;
    out->active_resistance = broadening.active > 0.0 ? broadening.resistance : NAN;
    out->active_support = broadening.active > 0.0 ? broadening.support : NAN;
    out->resistance_pivot = scan->highs.y2;
    out->resistance_pivot_index = (double)scan->highs.x2;
    out->support_pivot = scan->lows.y2;
    out->support_pivot_index = (double)scan->lows.x2;
    out->active_index = (double)idx;
    out->width = broadening.width;
    out->resistance_slope = broadening.resistance_slope;
    out->support_slope = broadening.support_slope;
    out->active = broadening.active;
    out->rising = broadening.rising;
    out->falling = broadening.falling;
    out->breakout =
        (isfinite(trigger_value) && out->active > 0.0 && trigger_value > out->resistance)
            ? 1.0
            : 0.0;
    out->breakdown =
        (isfinite(trigger_value) && out->active > 0.0 && trigger_value < out->support)
            ? 1.0
            : 0.0;
    out->direction = out->breakout > 0.0 ? 1.0 : (out->breakdown > 0.0 ? -1.0 : 0.0);
    if (isfinite(trigger_value) &&
        fabs(trigger_value) > 1.0e-9 &&
        out->width / fabs(trigger_value) > CXTA_STRUCT_BROADENING_MAX_WIDTH_TO_PRICE) {
        out->active = 0.0;
        out->active_resistance = NAN;
        out->active_support = NAN;
        out->rising = 0.0;
        out->falling = 0.0;
        out->breakout = 0.0;
        out->breakdown = 0.0;
        out->direction = 0.0;
    }
    if (out->active > 0.0 &&
        out->direction == 0.0 &&
        !cxta_struct_broadening_current_edges_are_tight(current, out)) {
        out->active = 0.0;
        out->active_resistance = NAN;
        out->active_support = NAN;
        out->rising = 0.0;
        out->falling = 0.0;
        out->breakout = 0.0;
        out->breakdown = 0.0;
        out->direction = 0.0;
    }
    if (out->active > 0.0 && out->width > 0.0) {
        const double slope_spread = fabs(out->resistance_slope - out->support_slope);
        const double breakout_boost = out->direction != 0.0 ? 1.0 : 0.0;
        out->strength = (out->width * 0.01) + slope_spread + breakout_boost;
    }
}

static double cxta_struct_broadening_project_resistance(
    const cxta_struct_broadening_scan* scan,
    size_t x) {
    return cxta_ts_wedge_project(
        (double)scan->highs.x1,
        scan->highs.y1,
        (double)scan->highs.x2,
        scan->highs.y2,
        (double)x);
}

static double cxta_struct_broadening_project_support(
    const cxta_struct_broadening_scan* scan,
    size_t x) {
    return cxta_ts_wedge_project(
        (double)scan->lows.x1,
        scan->lows.y1,
        (double)scan->lows.x2,
        scan->lows.y2,
        (double)x);
}

static double cxta_struct_broadening_fit_penalty(const cxta_series_bar_view* view,
                                                 const cxta_struct_broadening_scan* scan,
                                                 size_t idx) {
    const size_t start_high = scan->highs.x1 < scan->highs.x2 ? scan->highs.x1 : scan->highs.x2;
    const size_t start_low = scan->lows.x1 < scan->lows.x2 ? scan->lows.x1 : scan->lows.x2;
    const size_t start = start_high < start_low ? start_high : start_low;
    const size_t end = idx > 0u ? idx - 1u : idx;
    double penalty = 0.0;
    size_t i;

    if (!view || !scan || start > end) return 0.0;
    for (i = start; i <= end; ++i) {
        const cxta_series_bar* bar = cxta_series_bar_view_at(view, i);
        const double resistance = cxta_struct_broadening_project_resistance(scan, i);
        const double support = cxta_struct_broadening_project_support(scan, i);
        const double width = resistance - support;
        double high_excess;
        double low_excess;

        if (!bar || !(width > 0.0)) {
            penalty += 10.0;
            continue;
        }

        high_excess = bar->high - resistance;
        low_excess = support - bar->low;
        if (high_excess > 0.0) penalty += high_excess / width;
        if (low_excess > 0.0) penalty += low_excess / width;
    }
    return penalty;
}

static double cxta_struct_broadening_score(const cxta_series_bar_view* view,
                                           const cxta_struct_broadening_scan* scan,
                                           const cxta_struct_broadening_state* out,
                                           size_t idx,
                                           double trigger_value) {
    size_t last_pivot;
    size_t pivot_age;
    double upper_span;
    double lower_span;
    double min_span;
    double slope_gap;
    double width_ratio;
    double center;
    double price_distance;
    double fit_penalty;
    double edge_gap;
    double recency_score;
    double breakout_score;
    const cxta_series_bar* current = cxta_series_bar_view_current(view);

    if (!scan || !out || !out->has_pivots) return -1.0e100;
    if (!(out->width > 0.0) || out->support >= out->resistance) return -1.0e100;

    last_pivot = (scan->highs.x2 > scan->lows.x2) ? scan->highs.x2 : scan->lows.x2;
    pivot_age = (idx > last_pivot) ? (idx - last_pivot) : 0u;
    upper_span = (double)(scan->highs.x2 - scan->highs.x1);
    lower_span = (double)(scan->lows.x2 - scan->lows.x1);
    min_span = (upper_span < lower_span) ? upper_span : lower_span;
    slope_gap = fabs(out->resistance_slope - out->support_slope);
    width_ratio = isfinite(trigger_value) && fabs(trigger_value) > 1.0e-9
                      ? out->width / fabs(trigger_value)
                      : 1.0;
    center = (out->resistance + out->support) * 0.5;
    price_distance = isfinite(trigger_value) && out->width > 0.0
                         ? fabs(trigger_value - center) / out->width
                         : 1.0;
    fit_penalty = cxta_struct_broadening_fit_penalty(view, scan, idx);
    edge_gap = cxta_struct_broadening_current_edge_gap(current, out);
    recency_score = 250.0 / (1.0 + (double)pivot_age);
    breakout_score = (out->breakdown > 0.5 || out->breakout > 0.5) ? 60.0 : 0.0;

    if (width_ratio > CXTA_STRUCT_BROADENING_MAX_WIDTH_TO_PRICE) return -1.0e100;
    if (out->direction == 0.0 && !cxta_struct_broadening_current_edges_are_tight(current, out)) {
        return -1.0e100;
    }

    return (out->active > 0.5 ? 1000.0 : 0.0) +
           breakout_score +
           recency_score +
           (min_span * 4.0) +
           (20.0 * slope_gap) -
           (600.0 * fit_penalty) -
           (600.0 * edge_gap) -
           (300.0 * price_distance) -
           (1200.0 * width_ratio);
}

static cxta_struct_broadening_scored cxta_struct_broadening_auto_detect(
    const cxta_series_bar_view* view,
    size_t first,
    size_t last,
    size_t idx,
    int lookback,
    double min_width_ratio,
    int breakout_source,
    double trigger_value,
    int has_trigger_value) {
    cxta_struct_broadening_scored best = {0};
    const int max_strength = ((lookback / 4) > 8) ? 8 : ((lookback / 4) < 2 ? 2 : (lookback / 4));
    int left;
    int right;

    for (left = 1; left <= max_strength; ++left) {
        for (right = 1; right <= max_strength; ++right) {
            cxta_struct_broadening_scan scan;
            const size_t left_n = (size_t)left;
            const size_t right_n = (size_t)right;
            const size_t candidate_first = (first > left_n) ? first : left_n;
            const size_t candidate_last = idx - right_n;
            size_t high1;

            if (idx < (size_t)(left + right + 2)) continue;
            if (candidate_last < candidate_first) continue;

            scan = cxta_struct_broadening_scan_pivots(view, candidate_first, candidate_last, left, right);
            if (scan.high_count < 2u || scan.low_count < 2u) {
                cxta_struct_broadening_state state = {0};
                double score;

                cxta_struct_broadening_make_from_scan(
                    view,
                    idx,
                    &scan,
                    min_width_ratio,
                    breakout_source,
                    trigger_value,
                    has_trigger_value,
                    &state);
                score = cxta_struct_broadening_score(view, &scan, &state, idx, trigger_value);
                if (!best.valid || score > best.score) {
                    best.valid = 1;
                    best.score = score;
                    best.state = state;
                }
                continue;
            }

            for (high1 = 0u; high1 + 1u < scan.high_count; ++high1) {
                size_t high2;
                for (high2 = high1 + 1u; high2 < scan.high_count; ++high2) {
                    size_t low1;
                    for (low1 = 0u; low1 + 1u < scan.low_count; ++low1) {
                        size_t low2;
                        for (low2 = low1 + 1u; low2 < scan.low_count; ++low2) {
                            const cxta_struct_broadening_scan candidate =
                                cxta_struct_broadening_scan_from_pairs(
                                    &scan, high1, high2, low1, low2);
                            cxta_struct_broadening_state state = {0};
                            double score;

                            cxta_struct_broadening_make_from_scan(
                                view,
                                idx,
                                &candidate,
                                min_width_ratio,
                                breakout_source,
                                trigger_value,
                                has_trigger_value,
                                &state);
                            score = cxta_struct_broadening_score(
                                view, &candidate, &state, idx, trigger_value);
                            if (!best.valid || score > best.score) {
                                best.valid = 1;
                                best.score = score;
                                best.state = state;
                            }
                        }
                    }
                }
            }
        }
    }

    return best;
}

int cxta_struct_broadening_args_parse(const double* args,
                                      size_t nargs,
                                      size_t current_index,
                                      cxta_struct_broadening_args* out) {
    const int raw_left = cxta_struct_broadening_read_int_arg(args, nargs, 0u, 2);
    const int raw_right = cxta_struct_broadening_read_int_arg(args, nargs, 1u, 2);
    const int raw_breakout_source = cxta_struct_broadening_read_int_arg(args, nargs, 4u, 0);
    (void)current_index;

    if (!out) return 0;

    out->auto_left_right = (raw_left <= 0 || raw_right <= 0) ? 1 : 0;
    out->left = cxta_ts_clamp_period(raw_left);
    out->right = cxta_ts_clamp_period(raw_right);
    out->lookback = cxta_ts_clamp_period(cxta_struct_broadening_read_int_arg(args, nargs, 2u, 40));
    out->min_width_ratio = cxta_struct_broadening_read_positive_arg(args, nargs, 3u, 1.15);
    out->breakout_source = raw_breakout_source == 1 ? 1 : 0;
    out->trigger_value = NAN;
    out->has_trigger_value = 0;

    if (out->left < 1) out->left = 1;
    if (out->right < 1) out->right = 1;
    if (out->lookback < 1) out->lookback = 1;
    return 1;
}

int cxta_struct_broadening_state_compute(const cxta_series_bar_view* view,
                                         const cxta_struct_broadening_args* args,
                                         cxta_struct_broadening_state* out) {
    cxta_struct_broadening_args parsed = {0};
    size_t idx;
    size_t start;
    size_t first;
    size_t last;

    if (!out) return 0;
    cxta_struct_broadening_seed_defaults(view, out);
    if (!cxta_series_bar_view_valid(view)) return 1;

    if (args) {
        parsed = *args;
    } else if (!cxta_struct_broadening_args_parse(NULL, 0u, view->index, &parsed)) {
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
        const cxta_struct_broadening_scored best =
            cxta_struct_broadening_auto_detect(
                view, start, last, idx, parsed.lookback, parsed.min_width_ratio,
                parsed.breakout_source, parsed.trigger_value, parsed.has_trigger_value);
        if (best.valid) *out = best.state;
        return 1;
    }

    {
        const cxta_struct_broadening_scan scan =
            cxta_struct_broadening_scan_pivots(view, first, last, parsed.left, parsed.right);
        cxta_struct_broadening_make_from_scan(
            view,
            idx,
            &scan,
            parsed.min_width_ratio,
            parsed.breakout_source,
            parsed.trigger_value,
            parsed.has_trigger_value,
            out);
    }
    return 1;
}

void cxta_struct_broadening_state_mask_geometry(cxta_struct_broadening_state* out) {
    if (!out) return;
    if (out->has_pivots) return;

    out->resistance = NAN;
    out->support = NAN;
    out->active_resistance = NAN;
    out->active_support = NAN;
    out->resistance_pivot = NAN;
    out->resistance_pivot_index = NAN;
    out->support_pivot = NAN;
    out->support_pivot_index = NAN;
    out->active_index = NAN;
    out->width = NAN;
    out->resistance_slope = NAN;
    out->support_slope = NAN;
}
