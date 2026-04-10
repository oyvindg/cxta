/**
 * @file rma.h
 * @brief Wilder RMA helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for the RMA indicator. */
static const cxta_param_descriptor cxta_rma_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for RMA. */
static const cxta_bridge_fn_spec cxta_rma_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("rma", 1u, 1u, cxta_rma_params, 1);

/** @brief Expression-facing descriptor for RMA. */
extern const cxta_indicator_descriptor cxta_rma_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief RMA state for incremental updates.
 */
typedef struct {
    double value;       /**< Current RMA value. */
    double initialized; /**< Non-zero once seeded. */
} cxta_rma_state;

/**
 * @brief Perform one RMA update.
 * @param[in] sample Current sample.
 * @param[in] period RMA period.
 * @param[in,out] st RMA state.
 * @return Updated RMA value.
 */
double cxta_rma_step(double sample, int period, cxta_rma_state* st);

/**
 * @brief Compute close-based RMA at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period RMA period.
 * @return RMA value, or 0.0 when the view is invalid.
 */
double cxta_rma(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
