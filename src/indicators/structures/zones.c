/**
 * @file zones.c
 * @brief Descriptor metadata for structure zone indicators.
 */

#include "common.h"

#include <cxta/indicators/macros.h>
#include <cxta/indicators/structures/zones.h>
#include <cxta/structure/fvg.h>
#include <cxta/structure/liquidity.h>
#include <cxta/structure/order_block.h>
#include <cxta/structure/pivot.h>

#include <math.h>

typedef struct cxta_liquidity_pools_state {
    double nearest_buy_pool;
    double nearest_sell_pool;
    double pool_strength;
    double swept;
} cxta_liquidity_pools_state;

typedef struct cxta_liquidity_pool_cluster {
    double level;
    int revisit_count;
    size_t last_pivot_index;
    int is_buy;
} cxta_liquidity_pool_cluster;

static const cxta_field_descriptor cxta_fvg_fields[] = {
    CXTA_STRUCT_FIELD_HIDDEN("bullish", cxta_struct_fvg_state, bullish),
    CXTA_STRUCT_FIELD_HIDDEN("bearish", cxta_struct_fvg_state, bearish),
    CXTA_STRUCT_FIELD("top", cxta_struct_fvg_state, top),
    CXTA_STRUCT_FIELD("bottom", cxta_struct_fvg_state, bottom),
    CXTA_STRUCT_FIELD("filled", cxta_struct_fvg_state, filled),
};

static const cxta_field_descriptor cxta_order_block_fields[] = {
    CXTA_STRUCT_FIELD("bullish", cxta_struct_order_block_state, bullish),
    CXTA_STRUCT_FIELD("bearish", cxta_struct_order_block_state, bearish),
    CXTA_STRUCT_FIELD("top", cxta_struct_order_block_state, top),
    CXTA_STRUCT_FIELD("bottom", cxta_struct_order_block_state, bottom),
    CXTA_STRUCT_FIELD("strength", cxta_struct_order_block_state, strength),
};

static const cxta_field_descriptor cxta_liquidity_fields[] = {
    CXTA_STRUCT_FIELD_HIDDEN("buy_side", cxta_struct_liquidity_state, buy_side),
    CXTA_STRUCT_FIELD_HIDDEN("sell_side", cxta_struct_liquidity_state, sell_side),
    CXTA_STRUCT_FIELD("level", cxta_struct_liquidity_state, level),
    CXTA_STRUCT_FIELD("swept", cxta_struct_liquidity_state, swept),
};

static const cxta_field_descriptor cxta_liquidity_pools_fields[] = {
    CXTA_STRUCT_FIELD("nearest_buy_pool", cxta_liquidity_pools_state, nearest_buy_pool),
    CXTA_STRUCT_FIELD("nearest_sell_pool", cxta_liquidity_pools_state, nearest_sell_pool),
    CXTA_STRUCT_FIELD("pool_strength", cxta_liquidity_pools_state, pool_strength),
    CXTA_STRUCT_FIELD("swept", cxta_liquidity_pools_state, swept),
};

static const cxta_plot_field_descriptor cxta_liquidity_pools_plot_fields[] = {
    CXTA_FIELD_PLOT("nearest_buy_pool", true, "Nearest Buy Pool", "liquidity_pools", "#22c55e", "zone", "levels", "Nearest buy-side liquidity pool.", "Drawn as a green liquidity zone over price candles."),
    CXTA_FIELD_PLOT("nearest_sell_pool", true, "Nearest Sell Pool", "liquidity_pools", "#ef4444", "zone", "levels", "Nearest sell-side liquidity pool.", "Drawn as a red liquidity zone over price candles."),
    CXTA_FIELD_PLOT("pool_strength", false, "Pool Strength", "liquidity_pools", "#94a3b8", "hidden", "levels", "Strength of the nearest active liquidity pool.", "Use in strategy filters rather than the default price overlay."),
    CXTA_FIELD_PLOT("swept", false, "Swept", "liquidity_pools", "#facc15", "hidden", "levels", "Sweep state for the active liquidity pool.", "Use as an event/exit condition rather than a continuous overlay."),
};

static const cxta_indicator_plot_descriptor cxta_liquidity_pools_plot_descriptor = {
    .indicator_name = "liquidity_pools",
    .fields = cxta_liquidity_pools_plot_fields,
    .field_count = CXTA_ARRAY_COUNT(cxta_liquidity_pools_plot_fields),
};

static const cxta_indicator_plot_descriptor cxta_fvg_plot_descriptor =
    CXTA_STRUCTURE_PLOT_DESCRIPTOR("fvg");
static const cxta_indicator_plot_descriptor cxta_order_block_plot_descriptor =
    CXTA_STRUCTURE_PLOT_DESCRIPTOR("order_block");
static const cxta_indicator_plot_descriptor cxta_liquidity_plot_descriptor =
    CXTA_STRUCTURE_PLOT_DESCRIPTOR("liquidity");

static void cxta_fvg_descriptor_eval(const cxta_series_bar_view* view,
                                     const double* args,
                                     size_t nargs,
                                     void* out) {
    cxta_struct_fvg_state value = {0};

    if (view && cxta_series_bar_view_valid(view)) {
        const int fallback = (int)cxta_series_clamp_index(view->size, view->index) + 1;
        const size_t lookback =
            (size_t)cxta_descriptor_period_arg(args, nargs, 0u, fallback);
        (void)cxta_struct_fvg_state_compute(view, lookback, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_order_block_descriptor_eval(const cxta_series_bar_view* view,
                                             const double* args,
                                             size_t nargs,
                                             void* out) {
    cxta_struct_order_block_state value = {0};
    cxta_struct_pivot_args parsed = {0};

    if (view && cxta_descriptor_parse_pivot_args(view, args, nargs, &parsed)) {
        (void)cxta_struct_order_block_state_compute(view, &parsed, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_liquidity_descriptor_eval(const cxta_series_bar_view* view,
                                           const double* args,
                                           size_t nargs,
                                           void* out) {
    cxta_struct_pivot_state pivot = {0};
    cxta_struct_liquidity_state value = {0};
    cxta_struct_pivot_args parsed = {0};
    const double tolerance = cxta_descriptor_double_arg(args, nargs, 3u, 0.001);

    if (view && cxta_descriptor_parse_pivot_args(view, args, nargs, &parsed)) {
        (void)cxta_struct_pivot_state_compute(view, &parsed, &pivot);
        (void)cxta_struct_liquidity_state_compute(view, &pivot, tolerance, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static int cxta_liquidity_pools_is_swing_high(const cxta_series_bar_view* view, size_t index) {
    const cxta_series_bar* prev;
    const cxta_series_bar* cur;
    const cxta_series_bar* next;
    if (!view || index == 0u || index + 1u >= view->size) return 0;
    prev = cxta_series_bar_view_at(view, index - 1u);
    cur = cxta_series_bar_view_at(view, index);
    next = cxta_series_bar_view_at(view, index + 1u);
    return prev && cur && next && cur->high > prev->high && cur->high > next->high;
}

static int cxta_liquidity_pools_is_swing_low(const cxta_series_bar_view* view, size_t index) {
    const cxta_series_bar* prev;
    const cxta_series_bar* cur;
    const cxta_series_bar* next;
    if (!view || index == 0u || index + 1u >= view->size) return 0;
    prev = cxta_series_bar_view_at(view, index - 1u);
    cur = cxta_series_bar_view_at(view, index);
    next = cxta_series_bar_view_at(view, index + 1u);
    return prev && cur && next && cur->low < prev->low && cur->low < next->low;
}

static void cxta_liquidity_pools_add_cluster(
    cxta_liquidity_pool_cluster* pools,
    size_t* pool_count,
    size_t pool_cap,
    double level,
    size_t pivot_index,
    int is_buy,
    double threshold) {
    size_t i;
    if (!pools || !pool_count) return;
    for (i = 0u; i < *pool_count; ++i) {
        cxta_liquidity_pool_cluster* pool = &pools[i];
        double total;
        if (pool->is_buy != is_buy) continue;
        if (fabs(pool->level - level) > threshold) continue;
        total = (pool->level * (double)pool->revisit_count) + level;
        pool->revisit_count += 1;
        pool->level = total / (double)pool->revisit_count;
        pool->last_pivot_index = pivot_index;
        return;
    }
    if (*pool_count >= pool_cap) return;
    pools[*pool_count].level = level;
    pools[*pool_count].revisit_count = 1;
    pools[*pool_count].last_pivot_index = pivot_index;
    pools[*pool_count].is_buy = is_buy;
    *pool_count += 1u;
}

static int cxta_liquidity_pools_bar_sweeps(
    const cxta_series_bar* bar,
    const cxta_liquidity_pool_cluster* pool) {
    if (!bar || !pool) return 0;
    return pool->is_buy ? (bar->low < pool->level) : (bar->high > pool->level);
}

static void cxta_liquidity_pools_descriptor_eval(const cxta_series_bar_view* view,
                                                 const double* args,
                                                 size_t nargs,
                                                 void* out) {
    cxta_liquidity_pools_state value = {0};
    cxta_liquidity_pool_cluster pools[256];
    size_t pool_count = 0u;
    int period = cxta_descriptor_period_arg(args, nargs, 0u, 20);
    double tolerance = cxta_descriptor_double_arg(args, nargs, 1u, 0.001);
    double min_strength = cxta_descriptor_double_arg(args, nargs, 2u, 0.0);
    size_t current_index;
    size_t window;
    size_t window_start;
    size_t pivot_start;
    size_t i;
    const cxta_series_bar* current;
    double threshold;
    double buy_strength = 0.0;
    double sell_strength = 0.0;
    int has_buy_pool = 0;
    int has_sell_pool = 0;

    if (!out) return;
    if (!view || !cxta_series_bar_view_valid(view) || view->size < 3u || view->index < 2u) {
        cxta_descriptor_copy_struct(out, &value, sizeof(value));
        return;
    }

    current_index = view->index;
    current = cxta_series_bar_view_current(view);
    if (!current) {
        cxta_descriptor_copy_struct(out, &value, sizeof(value));
        return;
    }
    if (!isfinite(tolerance)) tolerance = 0.001;
    tolerance = fabs(tolerance);
    if (!isfinite(min_strength)) min_strength = 0.0;
    if (min_strength < 0.0) min_strength = 0.0;
    if (min_strength > 1.0) min_strength = 1.0;
    window = (size_t)(period < 1 ? 1 : period);
    if (window > current_index + 1u) window = current_index + 1u;
    window_start = current_index + 1u - window;
    pivot_start = window_start > 1u ? window_start : 1u;
    threshold = tolerance * fmax(fabs(current->close), 1e-12);

    for (i = pivot_start; i < current_index; ++i) {
        const cxta_series_bar* bar = cxta_series_bar_view_at(view, i);
        if (!bar) continue;
        if (cxta_liquidity_pools_is_swing_low(view, i)) {
            cxta_liquidity_pools_add_cluster(
                pools, &pool_count, CXTA_ARRAY_COUNT(pools), bar->low, i, 1, threshold);
        }
        if (cxta_liquidity_pools_is_swing_high(view, i)) {
            cxta_liquidity_pools_add_cluster(
                pools, &pool_count, CXTA_ARRAY_COUNT(pools), bar->high, i, 0, threshold);
        }
    }

    for (i = 0u; i < pool_count; ++i) {
        cxta_liquidity_pool_cluster* pool = &pools[i];
        int swept_before_current = 0;
        int swept_this_bar = 0;
        double strength = fmin((double)pool->revisit_count / 3.0, 1.0);
        size_t j;
        if (pool->last_pivot_index >= current_index) continue;
        if (strength < min_strength) continue;
        for (j = pool->last_pivot_index + 1u; j <= current_index; ++j) {
            const cxta_series_bar* bar = cxta_series_bar_view_at(view, j);
            if (!cxta_liquidity_pools_bar_sweeps(bar, pool)) continue;
            if (j == current_index) swept_this_bar = 1;
            else swept_before_current = 1;
            break;
        }
        if (swept_this_bar) value.swept = 1.0;
        if (swept_before_current || swept_this_bar) continue;

        if (pool->is_buy) {
            if (pool->level >= current->close) continue;
            if (!has_buy_pool || pool->level > value.nearest_buy_pool) {
                value.nearest_buy_pool = pool->level;
                buy_strength = strength;
                has_buy_pool = 1;
            }
        } else {
            if (pool->level <= current->close) continue;
            if (!has_sell_pool || pool->level < value.nearest_sell_pool) {
                value.nearest_sell_pool = pool->level;
                sell_strength = strength;
                has_sell_pool = 1;
            }
        }
    }

    if (has_buy_pool && has_sell_pool) {
        const double buy_distance = current->close - value.nearest_buy_pool;
        const double sell_distance = value.nearest_sell_pool - current->close;
        value.pool_strength = buy_distance <= sell_distance ? buy_strength : sell_strength;
    } else if (has_buy_pool) {
        value.pool_strength = buy_strength;
    } else if (has_sell_pool) {
        value.pool_strength = sell_strength;
    }

    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static const cxta_param_descriptor cxta_fvg_params[] = {
    {"lookback"},
};

static const cxta_param_descriptor cxta_order_block_params[] = {
    {"left"},
    {"right"},
    {"lookback"},
};

static const cxta_param_descriptor cxta_liquidity_params[] = {
    {"left"},
    {"right"},
    {"lookback"},
    {"tolerance"},
};

static const cxta_param_descriptor cxta_liquidity_pools_params[] = {
    {"period"},
    {"tolerance"},
    {"min_strength"},
};

const cxta_bridge_fn_spec cxta_fvg_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("fvg", 0u, 1u, cxta_fvg_params, 1);
const cxta_bridge_fn_spec cxta_order_block_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("order_block", 2u, 3u, cxta_order_block_params, 1);
const cxta_bridge_fn_spec cxta_liquidity_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("liquidity", 2u, 4u, cxta_liquidity_params, 1);
const cxta_bridge_fn_spec cxta_liquidity_pools_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("liquidity_pools", 0u, 3u, cxta_liquidity_pools_params, 1);

const cxta_indicator_descriptor cxta_fvg_descriptor =
    CXTA_STRUCTURE_DESCRIPTOR_WITH_PLOT("fvg",
                                        0,
                                        1,
                                        0,
                                        cxta_struct_fvg_state,
                                        0u,
                                        cxta_fvg_fields,
                                        cxta_fvg_descriptor_eval,
                                        NULL,
                                        cxta_fvg_params,
                                        &cxta_fvg_plot_descriptor);

const cxta_indicator_descriptor cxta_order_block_descriptor =
    CXTA_STRUCTURE_DESCRIPTOR_WITH_PLOT("order_block",
                                        2,
                                        3,
                                        0,
                                        cxta_struct_order_block_state,
                                        0u,
                                        cxta_order_block_fields,
                                        cxta_order_block_descriptor_eval,
                                        NULL,
                                        cxta_order_block_params,
                                        &cxta_order_block_plot_descriptor);

const cxta_indicator_descriptor cxta_liquidity_descriptor =
    CXTA_STRUCTURE_DESCRIPTOR_WITH_PLOT("liquidity",
                                        2,
                                        4,
                                        0,
                                        cxta_struct_liquidity_state,
                                        0u,
                                        cxta_liquidity_fields,
                                        cxta_liquidity_descriptor_eval,
                                        NULL,
                                        cxta_liquidity_params,
                                        &cxta_liquidity_plot_descriptor);

const cxta_indicator_descriptor cxta_liquidity_pools_descriptor =
    {
        "liquidity_pools",
        0,
        3,
        -1,
        -1,
        3,
        CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
        sizeof(cxta_liquidity_pools_state),
        0u,
        cxta_liquidity_pools_fields,
        CXTA_ARRAY_COUNT(cxta_liquidity_pools_fields),
        NULL,
        cxta_liquidity_pools_descriptor_eval,
        NULL,
        NULL,
        NULL,
        NULL,
        cxta_liquidity_pools_params,
        CXTA_ARRAY_COUNT(cxta_liquidity_pools_params),
        "liquidity_pools",
        &cxta_liquidity_pools_plot_descriptor,
    };
