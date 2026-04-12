/**
 * @file bop.h
 * @brief Balance of Power helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

extern const cxta_bridge_fn_spec cxta_bop_bridge_fn_spec;
extern const cxta_indicator_descriptor cxta_bop_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Balance of Power (C-O)/(H-L) for the current bar.
 *
 * Returns 0.0 when the high equals the low (no range).
 *
 * @param[in] view Bar series view.
 * @return BOP value in [-1, 1], or 0.0 when the view is invalid or bar has no range.
 */
double cxta_bop(const cxta_series_bar_view* view);

#ifdef __cplusplus
}
#endif
