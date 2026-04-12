/**
 * @file hvol.h
 * @brief Historical volatility helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_historical_volatility_params[] = {
    {"period"},
    {"annualization"},
};
static const cxta_bridge_fn_spec cxta_historical_volatility_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("historical_volatility", 1u, 2u, cxta_historical_volatility_params, 1);

extern const cxta_indicator_descriptor cxta_historical_volatility_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute annualised historical volatility at the current view index.
 *
 * Computes the population standard deviation of log returns over the last
 * @p period bars and scales by √252.
 *
 * @param[in] view   Bar series view (needs at least period+1 bars for meaningful result).
 * @param[in] period Number of log-return samples.
 * @return Annualised volatility (fraction, not percent), or 0.0 when invalid.
 */
double cxta_hvol(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
