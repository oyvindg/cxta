/**
 * @file highest.h
 * @brief Rolling maximum (highest) over a lookback window.
 *
 * Default bar-source evaluates the highest high; the scalar-source
 * variant accepts any series (e.g. `highest(atr(14), 20)`).
 */

#pragma once

#include "../indicators/descriptor.h"

/** @brief Named parameters for the highest indicator. */
static const cxta_param_descriptor cxta_highest_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for highest. */
static const cxta_bridge_fn_spec cxta_highest_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("highest", 1u, 1u, cxta_highest_params, 1);

/** @brief Expression-facing descriptor for highest. */
extern const cxta_indicator_descriptor cxta_highest_descriptor;
