/**
 * @file keltner.h
 * @brief Keltner channel helpers.
 */

#pragma once

#include "donchian.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for Keltner channels. */
static const cxta_param_descriptor cxta_keltner_params[] = {
    {"ema_period"},
    {"atr_mult"},
    {"atr_period"},
};

/** @brief Bridge-facing signature metadata for Keltner. */
static const cxta_bridge_fn_spec cxta_keltner_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("keltner", 2u, 3u, cxta_keltner_params, 1);

/** @brief Expression-facing descriptor for Keltner. */
extern const cxta_indicator_descriptor cxta_keltner_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute a Keltner channel at the current view index.
 * @param[in] view Bar series view.
 * @param[in] ema_period EMA period for the middle line.
 * @param[in] atr_mult ATR multiplier.
 * @param[in] atr_period ATR period.
 * @return Channel outputs.
 */
cxta_channel_output cxta_keltner(const cxta_series_bar_view* view,
                                 int ema_period,
                                 double atr_mult,
                                 int atr_period);

#ifdef __cplusplus
}
#endif
