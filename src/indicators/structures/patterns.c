/**
 * @file patterns.c
 * @brief Descriptor metadata for structure pattern indicators.
 */

#include "common.h"

#include <cxta/indicators/structures/patterns.h>
#include <cxta/structure/vwap.h>
#include <cxta/structure/wedge.h>

static const cxta_field_descriptor cxta_swing_anchor_vwap_fields[] = {
    CXTA_STRUCT_FIELD("value", cxta_struct_vwap_result, value),
    CXTA_STRUCT_FIELD("direction", cxta_struct_vwap_result, direction),
    CXTA_STRUCT_FIELD("reanchor", cxta_struct_vwap_result, reanchor),
    CXTA_STRUCT_FIELD("anchor_price", cxta_struct_vwap_result, anchor_price),
    CXTA_STRUCT_FIELD("anchor_bars", cxta_struct_vwap_result, anchor_bars),
    CXTA_STRUCT_FIELD("hh", cxta_struct_vwap_result, hh),
    CXTA_STRUCT_FIELD("hl", cxta_struct_vwap_result, hl),
    CXTA_STRUCT_FIELD("lh", cxta_struct_vwap_result, lh),
    CXTA_STRUCT_FIELD("ll", cxta_struct_vwap_result, ll),
    CXTA_STRUCT_FIELD("last_high", cxta_struct_vwap_result, last_high),
    CXTA_STRUCT_FIELD("last_low", cxta_struct_vwap_result, last_low),
    CXTA_STRUCT_FIELD("bull_vwap", cxta_struct_vwap_result, bull_vwap),
    CXTA_STRUCT_FIELD("bear_vwap", cxta_struct_vwap_result, bear_vwap),
};

static const cxta_field_descriptor cxta_wedge_fields[] = {
    CXTA_STRUCT_FIELD("upper", cxta_struct_wedge_state, upper),
    CXTA_STRUCT_FIELD("lower", cxta_struct_wedge_state, lower),
    CXTA_STRUCT_FIELD("width", cxta_struct_wedge_state, width),
    CXTA_STRUCT_FIELD("upperSlope", cxta_struct_wedge_state, upper_slope),
    CXTA_STRUCT_FIELD("lowerSlope", cxta_struct_wedge_state, lower_slope),
    CXTA_STRUCT_FIELD_HIDDEN("active", cxta_struct_wedge_state, active),
    CXTA_STRUCT_FIELD_HIDDEN("rising", cxta_struct_wedge_state, rising),
    CXTA_STRUCT_FIELD_HIDDEN("falling", cxta_struct_wedge_state, falling),
    CXTA_STRUCT_FIELD_HIDDEN("breakdown", cxta_struct_wedge_state, breakdown),
    CXTA_STRUCT_FIELD_HIDDEN("breakout", cxta_struct_wedge_state, breakout),
};

static void cxta_swing_anchor_vwap_descriptor_eval(const cxta_series_bar_view* view,
                                                   const double* args,
                                                   size_t nargs,
                                                   void* out) {
    cxta_struct_vwap_result value = {0};
    cxta_struct_vwap_args parsed = {0};

    if (view && cxta_struct_vwap_args_parse(args, nargs, &parsed)) {
        (void)cxta_struct_vwap_compute(view, &parsed, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_swing_anchor_vwap_descriptor_step(const cxta_series_bar_view* view,
                                                   size_t index,
                                                   const double* args,
                                                   size_t nargs,
                                                   void* state,
                                                   void* out) {
    cxta_struct_vwap_result value = {0};
    cxta_struct_vwap_args parsed = {0};

    if (view && state && cxta_struct_vwap_args_parse(args, nargs, &parsed)) {
        (void)cxta_struct_vwap_step(
            view, index, &parsed, (cxta_struct_vwap_state*)state, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_wedge_descriptor_eval(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs,
                                       void* out) {
    cxta_struct_wedge_state value = {0};
    cxta_struct_wedge_args parsed = {0};
    const size_t current_index =
        (view && view->size > 0u) ? cxta_series_clamp_index(view->size, view->index) : 0u;

    if (view && cxta_struct_wedge_args_parse(args, nargs, current_index, &parsed)) {
        (void)cxta_struct_wedge_state_compute(view, &parsed, &value);
        cxta_struct_wedge_state_mask_geometry(&value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static const cxta_param_descriptor cxta_swing_anchor_vwap_params[] = {
    {"swing_period"},
    {"apt"},
    {"use_adapt"},
    {"vol_bias"},
    {"atr_period"},
    {"min_apt"},
    {"max_apt"},
};

/** Default strings match `CXTA_STRUCT_VWAP_DEFAULT_*_STR` in `cxta/structure/vwap.h`. */
static const cxta_expr_arg_descriptor cxta_swing_anchor_vwap_expr_args[] = {
    {"swing_period", CXTA_EXPR_ARG_NUMERIC, CXTA_STRUCT_VWAP_DEFAULT_SWING_PERIOD_STR},
    {"apt", CXTA_EXPR_ARG_NUMERIC, CXTA_STRUCT_VWAP_DEFAULT_APT_STR},
    {"use_adapt", CXTA_EXPR_ARG_NUMERIC, CXTA_STRUCT_VWAP_DEFAULT_USE_ADAPT_STR},
    {"vol_bias", CXTA_EXPR_ARG_NUMERIC, CXTA_STRUCT_VWAP_DEFAULT_VOL_BIAS_STR},
    {"atr_period", CXTA_EXPR_ARG_NUMERIC, CXTA_STRUCT_VWAP_DEFAULT_ATR_PERIOD_STR},
    {"min_apt", CXTA_EXPR_ARG_NUMERIC, CXTA_STRUCT_VWAP_DEFAULT_MIN_APT_STR},
    {"max_apt", CXTA_EXPR_ARG_NUMERIC, CXTA_STRUCT_VWAP_DEFAULT_MAX_APT_STR},
};

static const cxta_param_descriptor cxta_wedge_params[] = {
    {"left"},
    {"right"},
    {"lookback"},
    {"max_width_ratio"},
};

const cxta_bridge_fn_spec cxta_swing_anchor_vwap_bridge_fn_spec = CXTA_BRIDGE_FN_SPEC_EXPR(
    "swing_anchor_vwap",
    0u,
    7u,
    cxta_swing_anchor_vwap_params,
    cxta_swing_anchor_vwap_expr_args,
    1);
const cxta_bridge_fn_spec cxta_wedge_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("wedge", 2u, 4u, cxta_wedge_params, 1);

const cxta_indicator_descriptor cxta_swing_anchor_vwap_descriptor =
    CXTA_STRUCTURE_DESCRIPTOR("swing_anchor_vwap",
                              0,
                              7,
                              0,
                              cxta_struct_vwap_result,
                              sizeof(cxta_struct_vwap_state),
                              cxta_swing_anchor_vwap_fields,
                              cxta_swing_anchor_vwap_descriptor_eval,
                              cxta_swing_anchor_vwap_descriptor_step,
                              cxta_swing_anchor_vwap_params);

const cxta_indicator_descriptor cxta_wedge_descriptor =
    CXTA_STRUCTURE_DESCRIPTOR("wedge",
                              2,
                              4,
                              5,
                              cxta_struct_wedge_state,
                              0u,
                              cxta_wedge_fields,
                              cxta_wedge_descriptor_eval,
                              NULL,
                              cxta_wedge_params);
