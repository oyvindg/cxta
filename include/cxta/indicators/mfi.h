/**
 * @file mfi.h
 * @brief Money Flow Index helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for the MFI indicator. */
static const cxta_param_descriptor cxta_mfi_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for MFI. */
static const cxta_bridge_fn_spec cxta_mfi_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("mfi", 1u, 1u, cxta_mfi_params, 1);

/** @brief Expression-facing descriptor for MFI. */
extern const cxta_indicator_descriptor cxta_mfi_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Money Flow Index at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return MFI in [0, 100], or 0.0 when invalid.
 */
double cxta_mfi(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
