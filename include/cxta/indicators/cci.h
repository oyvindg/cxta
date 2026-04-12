/**
 * @file cci.h
 * @brief Commodity Channel Index helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for the CCI indicator. */
static const cxta_param_descriptor cxta_cci_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for CCI. */
static const cxta_bridge_fn_spec cxta_cci_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("cci", 1u, 1u, cxta_cci_params, 1);

/** @brief Expression-facing descriptor for CCI. */
extern const cxta_indicator_descriptor cxta_cci_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute CCI at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return CCI value, or 0.0 when invalid.
 */
double cxta_cci(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
