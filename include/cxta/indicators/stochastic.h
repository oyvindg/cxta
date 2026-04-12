/**
 * @file stochastic.h
 * @brief Stochastic oscillator helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_stochastic_params[] = {
    {"k_period"},
    {"d_period"},
    {"smooth_k"},
};

static const cxta_bridge_fn_spec cxta_stochastic_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("stochastic", 1u, 3u, cxta_stochastic_params, 1);

extern const cxta_indicator_descriptor cxta_stochastic_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Stochastic output payload.
 */
typedef struct {
    double k; /**< Smoothed %K. */
    double d; /**< Smoothed %D. */
} cxta_stoch_output;

/**
 * @brief Compute stochastic outputs at the current view index.
 * @param[in] view Bar series view.
 * @param[in] k_period Lookback for the raw %K.
 * @param[in] d_period Smoothing period for %D.
 * @param[in] smooth_k Smoothing period for %K.
 * @return Stochastic outputs, or a zero-initialized struct when invalid.
 */
cxta_stoch_output cxta_stochastic(const cxta_series_bar_view* view,
                                  int k_period,
                                  int d_period,
                                  int smooth_k);

#ifdef __cplusplus
}
#endif
