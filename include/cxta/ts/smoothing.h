/**
 * @file smoothing.h
 * @brief Time-series smoothing primitives.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Clamp a smoothing period to the minimum valid value.
 * @param[in] period Requested period.
 * @return `period` when it is at least 1, otherwise 1.
 */
int cxta_ts_clamp_period(int period);

/**
 * @brief Compute the EMA smoothing factor for a period.
 * @param[in] period EMA period.
 * @return EMA alpha in `(0, 1]`.
 */
double cxta_ts_ema_alpha(int period);

/**
 * @brief Advance one exponential moving average step.
 * @param[in] prev Previous EMA value.
 * @param[in] x Current input value.
 * @param[in] period EMA period.
 * @return Updated EMA value.
 */
double cxta_ts_ema_step(double prev, double x, int period);

/**
 * @brief Advance one Wilder smoothing step.
 * @param[in] prev Previous smoothed value.
 * @param[in] x Current input value.
 * @param[in] period Wilder period.
 * @return Updated Wilder-smoothed value.
 */
double cxta_ts_wilder_step(double prev, double x, int period);

/**
 * @brief Split a signed delta into gain and loss components.
 * @param[in] diff Signed change between consecutive values.
 * @param[out] gain Receives the positive component, if non-null.
 * @param[out] loss Receives the absolute negative component, if non-null.
 */
void cxta_ts_gain_loss(double diff, double* gain, double* loss);

/**
 * @brief Update average gain and loss with Wilder smoothing.
 * @param[in,out] avg_gain Running average gain.
 * @param[in,out] avg_loss Running average loss.
 * @param[in] diff Signed change between consecutive values.
 * @param[in] period Wilder period.
 */
void cxta_ts_update_gain_loss(double* avg_gain, double* avg_loss, double diff, int period);

#ifdef __cplusplus
}
#endif
