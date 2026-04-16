/**
 * @file pivots.c
 * @brief Descriptor metadata for pivot-derived structure indicators.
 */

#include "common.h"

#include <cxta/indicators/structures/pivots.h>
#include <cxta/structure/pivot.h>
#include <cxta/structure/pivot_points.h>
#include <cxta/structure/sfp.h>

static const cxta_field_descriptor cxta_swing_pivots_fields[] = {
    CXTA_STRUCT_FIELD("high", cxta_struct_pivot_state, high),
    CXTA_STRUCT_FIELD("low", cxta_struct_pivot_state, low),
    CXTA_STRUCT_FIELD("is_high", cxta_struct_pivot_state, is_high),
    CXTA_STRUCT_FIELD("is_low", cxta_struct_pivot_state, is_low),
};

static const cxta_field_descriptor cxta_pivot_points_fields[] = {
    CXTA_STRUCT_FIELD("pp", cxta_struct_pivot_points_result, pp),
    CXTA_STRUCT_FIELD("r1", cxta_struct_pivot_points_result, r1),
    CXTA_STRUCT_FIELD("r2", cxta_struct_pivot_points_result, r2),
    CXTA_STRUCT_FIELD("r3", cxta_struct_pivot_points_result, r3),
    CXTA_STRUCT_FIELD("s1", cxta_struct_pivot_points_result, s1),
    CXTA_STRUCT_FIELD("s2", cxta_struct_pivot_points_result, s2),
    CXTA_STRUCT_FIELD("s3", cxta_struct_pivot_points_result, s3),
};

static const cxta_field_descriptor cxta_sfp_fields[] = {
    CXTA_STRUCT_FIELD("sfp_high", cxta_struct_sfp_state, sfp_high),
    CXTA_STRUCT_FIELD("sfp_low", cxta_struct_sfp_state, sfp_low),
    CXTA_STRUCT_FIELD("level", cxta_struct_sfp_state, level),
};

static void cxta_swing_pivots_descriptor_eval(const cxta_series_bar_view* view,
                                              const double* args,
                                              size_t nargs,
                                              void* out) {
    cxta_struct_pivot_state value = {0};
    cxta_struct_pivot_args parsed = {0};

    if (view && cxta_descriptor_parse_pivot_args(view, args, nargs, &parsed)) {
        (void)cxta_struct_pivot_state_compute(view, &parsed, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_pivot_points_descriptor_eval(const cxta_series_bar_view* view,
                                              const double* args,
                                              size_t nargs,
                                              void* out) {
    cxta_struct_pivot_points_result value = {0};
    (void)args;
    (void)nargs;

    if (view) {
        (void)cxta_struct_pivot_points_compute(view, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_sfp_descriptor_eval(const cxta_series_bar_view* view,
                                     const double* args,
                                     size_t nargs,
                                     void* out) {
    cxta_struct_pivot_state pivot = {0};
    cxta_struct_sfp_state value = {0};
    cxta_struct_pivot_args parsed = {0};

    if (view && cxta_descriptor_parse_pivot_args(view, args, nargs, &parsed)) {
        (void)cxta_struct_pivot_state_compute(view, &parsed, &pivot);
        (void)cxta_struct_sfp_state_compute(view, &pivot, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static const cxta_param_descriptor cxta_swing_pivots_params[] = {
    {"left"},
    {"right"},
    {"lookback"},
};

static const cxta_param_descriptor cxta_sfp_params[] = {
    {"left"},
    {"right"},
    {"lookback"},
};

const cxta_bridge_fn_spec cxta_pivot_points_bridge_fn_spec = {
    "pivot_points",
    0u,
    0u,
    NULL,
    0u,
    NULL,
    0u,
    1,
};

const cxta_bridge_fn_spec cxta_swing_pivots_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("swing_pivots", 2u, 3u, cxta_swing_pivots_params, 1);
const cxta_bridge_fn_spec cxta_sfp_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("sfp", 2u, 3u, cxta_sfp_params, 1);

const cxta_indicator_descriptor cxta_swing_pivots_descriptor =
    CXTA_STRUCTURE_DESCRIPTOR("swing_pivots",
                              2,
                              3,
                              0,
                              cxta_struct_pivot_state,
                              0u,
                              cxta_swing_pivots_fields,
                              cxta_swing_pivots_descriptor_eval,
                              NULL,
                              cxta_swing_pivots_params);

const cxta_indicator_descriptor cxta_pivot_points_descriptor =
    CXTA_STRUCTURE_DESCRIPTOR_NO_PARAMS("pivot_points",
                                        0,
                                        0,
                                        0,
                                        cxta_struct_pivot_points_result,
                                        0u,
                                        cxta_pivot_points_fields,
                                        cxta_pivot_points_descriptor_eval,
                                        NULL);

const cxta_indicator_descriptor cxta_sfp_descriptor =
    CXTA_STRUCTURE_DESCRIPTOR("sfp",
                              2,
                              3,
                              0,
                              cxta_struct_sfp_state,
                              0u,
                              cxta_sfp_fields,
                              cxta_sfp_descriptor_eval,
                              NULL,
                              cxta_sfp_params);
