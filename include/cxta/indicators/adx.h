/**
 * @file adx.h
 * @brief Average Directional Index helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for ADX. */
static const cxta_param_descriptor cxta_adx_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for ADX. */
static const cxta_bridge_fn_spec cxta_adx_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("adx", 1u, 1u, cxta_adx_params, 1);

/** @brief Expression-facing descriptor for ADX. */
extern const cxta_indicator_descriptor cxta_adx_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ADX output payload.
 */
typedef struct {
    double adx;
    double plus_di;
    double minus_di;
} cxta_adx_output;

/**
 * @brief ADX state for incremental updates.
 */
typedef struct {
    double plus_dm_sm;
    double minus_dm_sm;
    double tr_sm;
    double adx;
} cxta_adx_state;

/**
 * @brief Perform one ADX update.
 * @param[in] plus_dm Positive directional movement sample.
 * @param[in] minus_dm Negative directional movement sample.
 * @param[in] tr True range sample.
 * @param[in] period ADX period.
 * @param[in,out] st ADX state.
 * @return ADX outputs.
 */
cxta_adx_output cxta_adx_step(double plus_dm,
                              double minus_dm,
                              double tr,
                              int period,
                              cxta_adx_state* st);

/**
 * @brief Compute ADX at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period ADX period.
 * @return ADX outputs.
 */
cxta_adx_output cxta_adx(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
