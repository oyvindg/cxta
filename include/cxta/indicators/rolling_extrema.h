/**
 * @file rolling_extrema.h
 * @brief Rolling window min/max descriptors (bar field and scalar-source forms).
 */

#pragma once

#include "../indicators/descriptor.h"

/** @brief Named parameters for rolling window extrema indicators. */
static const cxta_param_descriptor cxta_rolling_extrema_params[] = {
    {"period"},
};

/** @brief Bridge metadata for `rolling_max`. */
static const cxta_bridge_fn_spec cxta_rolling_max_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("rolling_max", 1u, 1u, cxta_rolling_extrema_params, 1);

/** @brief Bridge metadata for `rolling_min`. */
static const cxta_bridge_fn_spec cxta_rolling_min_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("rolling_min", 1u, 1u, cxta_rolling_extrema_params, 1);

/** @brief Bridge metadata for `rolling_max_close`. */
static const cxta_bridge_fn_spec cxta_rolling_max_close_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("rolling_max_close", 1u, 1u, cxta_rolling_extrema_params, 1);

/** @brief Bridge metadata for `rolling_min_close`. */
static const cxta_bridge_fn_spec cxta_rolling_min_close_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("rolling_min_close", 1u, 1u, cxta_rolling_extrema_params, 1);

extern const cxta_indicator_descriptor cxta_rolling_max_descriptor;
extern const cxta_indicator_descriptor cxta_rolling_min_descriptor;
extern const cxta_indicator_descriptor cxta_rolling_max_close_descriptor;
extern const cxta_indicator_descriptor cxta_rolling_min_close_descriptor;
