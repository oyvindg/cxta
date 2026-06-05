/**
 * @file broadening.c
 * @brief Descriptor metadata for broadening channel pattern detection.
 */

#include "../common.h"

#include <cxta/indicators/macros.h>
#include <cxta/indicators/structures/patterns.h>
#include <cxta/structure/wedge.h>
#include <cxta/ts/smoothing.h>

#include <math.h>

static const cxta_field_descriptor cxta_broadening_fields[] = {
    CXTA_STRUCT_FIELD("resistance", cxta_struct_broadening_state, resistance),
    CXTA_STRUCT_FIELD("support", cxta_struct_broadening_state, support),
    CXTA_STRUCT_FIELD("activeResistance", cxta_struct_broadening_state, active_resistance),
    CXTA_STRUCT_FIELD("activeSupport", cxta_struct_broadening_state, active_support),
    CXTA_STRUCT_FIELD_HIDDEN("resistancePivot", cxta_struct_broadening_state, resistance_pivot),
    CXTA_STRUCT_FIELD_HIDDEN("resistancePivotIndex", cxta_struct_broadening_state, resistance_pivot_index),
    CXTA_STRUCT_FIELD_HIDDEN("supportPivot", cxta_struct_broadening_state, support_pivot),
    CXTA_STRUCT_FIELD_HIDDEN("supportPivotIndex", cxta_struct_broadening_state, support_pivot_index),
    CXTA_STRUCT_FIELD_HIDDEN("activeIndex", cxta_struct_broadening_state, active_index),
    CXTA_STRUCT_FIELD("width", cxta_struct_broadening_state, width),
    CXTA_STRUCT_FIELD("resistanceSlope", cxta_struct_broadening_state, resistance_slope),
    CXTA_STRUCT_FIELD("supportSlope", cxta_struct_broadening_state, support_slope),
    CXTA_STRUCT_FIELD_HIDDEN("active", cxta_struct_broadening_state, active),
    CXTA_STRUCT_FIELD_HIDDEN("rising", cxta_struct_broadening_state, rising),
    CXTA_STRUCT_FIELD_HIDDEN("falling", cxta_struct_broadening_state, falling),
    CXTA_STRUCT_FIELD_HIDDEN("breakout", cxta_struct_broadening_state, breakout),
    CXTA_STRUCT_FIELD_HIDDEN("breakdown", cxta_struct_broadening_state, breakdown),
    CXTA_STRUCT_FIELD_HIDDEN("direction", cxta_struct_broadening_state, direction),
    CXTA_STRUCT_FIELD_HIDDEN("strength", cxta_struct_broadening_state, strength),
};

static const cxta_plot_field_descriptor cxta_broadening_plot_fields[] = {
    {
        .field_name = "resistance",
        .auto_plot = true,
        .label = "Broadening Resistance",
        .pane = "broadening",
        .color = "#22c55e",
        .style = "zigzag",
        .scale = "price",
        .pivot_value_field = "resistancePivot",
        .pivot_index_field = "resistancePivotIndex",
        .active_value_field = "resistance",
        .active_index_field = "activeIndex",
        .pivot_zero_is_missing = true,
        .active_zero_is_missing = true,
        .hover_summary = "Projected upper resistance candidate for the current broadening setup.",
        .hover_indication = "A breakout is trigger price above this line.",
        .show_price = true,
    },
    {
        .field_name = "support",
        .auto_plot = true,
        .label = "Broadening Support",
        .pane = "broadening",
        .color = "#ef4444",
        .style = "zigzag",
        .scale = "price",
        .pivot_value_field = "supportPivot",
        .pivot_index_field = "supportPivotIndex",
        .active_value_field = "support",
        .active_index_field = "activeIndex",
        .pivot_zero_is_missing = true,
        .active_zero_is_missing = true,
        .hover_summary = "Projected lower support candidate for the current broadening setup.",
        .hover_indication = "A breakdown is trigger price below this line.",
        .show_price = true,
    },
    CXTA_FIELD_PLOT("width", false, "Broadening Width", "broadening", "#38bdf8", "line", "price", "Distance between projected resistance and support.", "Rising width confirms channel expansion."),
    CXTA_FIELD_PLOT("activeResistance", false, "Active Broadening Resistance", "broadening", "#86efac", "line", "price", "Projected resistance when broadening geometry is active.", "Use with activeSupport to inspect signal-qualified channel boundaries."),
    CXTA_FIELD_PLOT("activeSupport", false, "Active Broadening Support", "broadening", "#fca5a5", "line", "price", "Projected support when broadening geometry is active.", "Use with activeResistance to inspect signal-qualified channel boundaries."),
    CXTA_FIELD_PLOT("resistanceSlope", false, "Resistance Slope", "broadening", "#22c55e", "line", "price", "Slope of the broadening resistance line.", "Compare with support slope to classify broadening geometry."),
    CXTA_FIELD_PLOT("supportSlope", false, "Support Slope", "broadening", "#ef4444", "line", "price", "Slope of the broadening support line.", "Compare with resistance slope to classify broadening geometry."),
    CXTA_FIELD_PLOT("active", false, "Broadening Active", "broadening", "#f59e0b", "line", "price", "Pattern geometry is broadening enough to be active.", "Gate breakout and breakdown signals with active > 0."),
    CXTA_FIELD_PLOT("rising", false, "Rising Broadening", "broadening", "#22c55e", "line", "price", "Both channel lines rise while resistance rises faster than support.", "Use for ascending broadening-channel setups."),
    CXTA_FIELD_PLOT("falling", false, "Falling Broadening", "broadening", "#ef4444", "line", "price", "Both channel lines fall while support falls faster than resistance.", "Use for descending broadening-channel setups."),
    CXTA_FIELD_PLOT("breakout", false, "Broadening Breakout", "broadening", "#22c55e", "line", "price", "Trigger price crossed above resistance.", "This is the upward breakout flag."),
    CXTA_FIELD_PLOT("breakdown", false, "Broadening Breakdown", "broadening", "#ef4444", "line", "price", "Trigger price crossed below support.", "This is the downward breakdown flag."),
    CXTA_FIELD_PLOT("direction", false, "Broadening Direction", "broadening", "#f59e0b", "line", "price", "Breakout direction: 1 up, -1 down, 0 none.", "Use when one expression should handle both directions."),
    CXTA_FIELD_PLOT("strength", false, "Broadening Strength", "broadening", "#38bdf8", "line", "price", "Geometry and breakout strength score.", "Use as a quality filter after active/breakout checks."),
};

static const cxta_indicator_plot_descriptor cxta_broadening_plot_descriptor = {
    .indicator_name = "broadening",
    .fields = cxta_broadening_plot_fields,
    .field_count = CXTA_ARRAY_COUNT(cxta_broadening_plot_fields),
};

static const cxta_param_descriptor cxta_broadening_params[] = {
    {"source"},
    {"lookback"},
    {"sensitivity"},
    {"min_width_ratio"},
};

static const cxta_expr_arg_descriptor cxta_broadening_expr_args[] = {
    {"source", CXTA_EXPR_ARG_NUMERIC, "close", "Current-bar trigger value for breakout/breakdown checks."},
    {"lookback", CXTA_EXPR_ARG_NUMERIC, "40", "Pivot scan lookback window."},
    {"sensitivity", CXTA_EXPR_ARG_NUMERIC, "0", "Pivot sensitivity; 0 keeps automatic pivot scanning."},
    {"min_width_ratio", CXTA_EXPR_ARG_NUMERIC, "1.15", "Minimum expansion ratio required for an active broadening pattern."},
};

static int cxta_broadening_args_are_public_source_form(const double* args, size_t nargs) {
    if (!args || nargs != 4u) return 0;
    return isfinite(args[0u]) &&
           isfinite(args[1u]) &&
           isfinite(args[2u]) &&
           isfinite(args[3u]) &&
           args[1u] >= 1.0 &&
           args[3u] > 0.0;
}

static int cxta_broadening_public_args_parse(const double* args,
                                             size_t nargs,
                                             size_t current_index,
                                             cxta_struct_broadening_args* out) {
    int sensitivity;

    if (!out || !cxta_broadening_args_are_public_source_form(args, nargs)) return 0;
    if (!cxta_struct_broadening_args_parse(NULL, 0u, current_index, out)) return 0;

    sensitivity = (int)llround(args[2u]);
    if (sensitivity < 0) sensitivity = 0;

    out->lookback = cxta_ts_clamp_period((int)llround(args[1u]));
    out->min_width_ratio = args[3u];
    out->trigger_value = args[0u];
    out->has_trigger_value = 1;
    if (sensitivity > 0) {
        out->left = cxta_ts_clamp_period(sensitivity);
        out->right = cxta_ts_clamp_period(sensitivity);
        out->auto_left_right = 0;
    } else {
        out->left = 1;
        out->right = 1;
        out->auto_left_right = 1;
    }
    return 1;
}

static void cxta_broadening_descriptor_eval(const cxta_series_bar_view* view,
                                            const double* args,
                                            size_t nargs,
                                            void* out) {
    cxta_struct_broadening_state value = {0};
    cxta_struct_broadening_args parsed = {0};
    int parsed_ok = 0;
    const size_t current_index =
        (view && view->size > 0u) ? cxta_series_clamp_index(view->size, view->index) : 0u;

    if (cxta_broadening_args_are_public_source_form(args, nargs)) {
        parsed_ok = cxta_broadening_public_args_parse(args, nargs, current_index, &parsed);
    } else if (view) {
        parsed_ok = cxta_struct_broadening_args_parse(args, nargs, current_index, &parsed);
    }

    if (view && parsed_ok) {
        (void)cxta_struct_broadening_state_compute(view, &parsed, &value);
        cxta_struct_broadening_state_mask_geometry(&value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

const cxta_bridge_fn_spec cxta_broadening_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC_EXPR(
        "broadening",
        0u,
        4u,
        cxta_broadening_params,
        cxta_broadening_expr_args,
        1);

const cxta_indicator_descriptor cxta_broadening_descriptor = {
    "broadening",
    0,
    10,
    -1,
    -1,
    10,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_struct_broadening_state),
    0u,
    cxta_broadening_fields,
    CXTA_ARRAY_COUNT(cxta_broadening_fields),
    NULL,
    cxta_broadening_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_broadening_params,
    CXTA_ARRAY_COUNT(cxta_broadening_params),
    "broadening",
    &cxta_broadening_plot_descriptor,
};
