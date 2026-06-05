/**
 * @file trend.c
 * @brief Descriptor metadata for trend and break-of-structure indicators.
 */

#include "common.h"

#include <cxta/indicators/structures/trend.h>
#include <cxta/structure/bos.h>
#include <cxta/structure/pivot.h>
#include <cxta/structure/structure.h>

static const cxta_field_descriptor cxta_structure_fields[] = {
    CXTA_STRUCT_FIELD("hh", cxta_struct_structure_state, hh),
    CXTA_STRUCT_FIELD("hl", cxta_struct_structure_state, hl),
    CXTA_STRUCT_FIELD("lh", cxta_struct_structure_state, lh),
    CXTA_STRUCT_FIELD("ll", cxta_struct_structure_state, ll),
    CXTA_STRUCT_FIELD("last_high", cxta_struct_structure_state, last_high),
    CXTA_STRUCT_FIELD("last_low", cxta_struct_structure_state, last_low),
    CXTA_STRUCT_FIELD("strength", cxta_struct_structure_state, strength),
};

static const cxta_field_descriptor cxta_bos_fields[] = {
    CXTA_STRUCT_FIELD("bos_up", cxta_struct_bos_state, bos_up),
    CXTA_STRUCT_FIELD("bos_down", cxta_struct_bos_state, bos_down),
    CXTA_STRUCT_FIELD("level", cxta_struct_bos_state, level),
    CXTA_STRUCT_FIELD("choch_up", cxta_struct_bos_state, choch_up),
    CXTA_STRUCT_FIELD("choch_down", cxta_struct_bos_state, choch_down),
};

static const cxta_indicator_plot_descriptor cxta_structure_plot_descriptor =
    CXTA_STRUCTURE_PLOT_DESCRIPTOR("structure");
static const cxta_indicator_plot_descriptor cxta_bos_plot_descriptor =
    CXTA_STRUCTURE_PLOT_DESCRIPTOR("bos");

static void cxta_structure_descriptor_eval(const cxta_series_bar_view* view,
                                           const double* args,
                                           size_t nargs,
                                           void* out) {
    cxta_struct_pivot_state pivot = {0};
    cxta_struct_structure_state value = {0};
    cxta_struct_pivot_args parsed = {0};

    if (view && cxta_descriptor_parse_pivot_args(view, args, nargs, &parsed)) {
        (void)cxta_struct_pivot_state_compute(view, &parsed, &pivot);
        (void)cxta_struct_structure_state_compute(&pivot, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_bos_descriptor_eval(const cxta_series_bar_view* view,
                                     const double* args,
                                     size_t nargs,
                                     void* out) {
    cxta_struct_pivot_state pivot = {0};
    cxta_struct_bos_state value = {0};
    cxta_struct_pivot_args parsed = {0};

    if (view && cxta_descriptor_parse_pivot_args(view, args, nargs, &parsed)) {
        (void)cxta_struct_pivot_state_compute(view, &parsed, &pivot);
        (void)cxta_struct_bos_state_compute(view, &pivot, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static const cxta_param_descriptor cxta_structure_params[] = {
    {"left"},
    {"right"},
    {"lookback"},
};

static const cxta_param_descriptor cxta_bos_params[] = {
    {"left"},
    {"right"},
    {"lookback"},
};

const cxta_bridge_fn_spec cxta_structure_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("structure", 2u, 3u, cxta_structure_params, 1);
const cxta_bridge_fn_spec cxta_bos_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("bos", 2u, 3u, cxta_bos_params, 1);

const cxta_indicator_descriptor cxta_structure_descriptor =
    CXTA_STRUCTURE_DESCRIPTOR_WITH_PLOT("structure",
                                        2,
                                        3,
                                        6,
                                        cxta_struct_structure_state,
                                        0u,
                                        cxta_structure_fields,
                                        cxta_structure_descriptor_eval,
                                        NULL,
                                        cxta_structure_params,
                                        &cxta_structure_plot_descriptor);

const cxta_indicator_descriptor cxta_bos_descriptor =
    CXTA_STRUCTURE_DESCRIPTOR_WITH_PLOT("bos",
                                        2,
                                        3,
                                        0,
                                        cxta_struct_bos_state,
                                        0u,
                                        cxta_bos_fields,
                                        cxta_bos_descriptor_eval,
                                        NULL,
                                        cxta_bos_params,
                                        &cxta_bos_plot_descriptor);
