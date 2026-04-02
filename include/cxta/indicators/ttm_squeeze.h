/**
 * @file ttm_squeeze.h
 * @brief TTM squeeze helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TTM squeeze output payload.
 */
typedef struct {
    double squeeze_on;
    double squeeze_off;
    double momentum;
} cxta_squeeze_output;

/**
 * @brief Compute TTM squeeze outputs at the current view index.
 * @param[in] view Bar series view.
 * @param[in] bb_period Bollinger period.
 * @param[in] bb_mult Bollinger standard-deviation multiplier.
 * @param[in] kc_period Keltner EMA/ATR period.
 * @param[in] kc_mult Keltner ATR multiplier.
 * @param[in] momentum_period Momentum SMA lookback.
 * @return TTM squeeze outputs, or a zero-initialized struct when invalid.
 */
cxta_squeeze_output cxta_ttm_squeeze(const cxta_series_bar_view* view,
                                     int bb_period,
                                     double bb_mult,
                                     int kc_period,
                                     double kc_mult,
                                     int momentum_period);

#ifdef __cplusplus
}
#endif
