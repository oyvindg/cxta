/**
 * @file lowest.h
 * @brief Rolling minimum (lowest) over a lookback window.
 *
 * Default bar-source evaluates the lowest low; the scalar-source
 * variant accepts any series (e.g. `lowest(rsi(14), 50)`).
 */

#pragma once

#include "../indicators/descriptor.h"

/** @brief Named parameters for the lowest indicator. */
static const cxta_param_descriptor cxta_lowest_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for lowest. */
static const cxta_bridge_fn_spec cxta_lowest_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("lowest", 1u, 1u, cxta_lowest_params, 1);

/** @brief Expression-facing descriptor for lowest. */
extern const cxta_indicator_descriptor cxta_lowest_descriptor;
