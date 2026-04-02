/**
 * @file patterns.h
 * @brief Scalar chart-pattern predicates.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Detect a confirmed double-top breakdown.
 * @param[in] peak1 First peak level.
 * @param[in] trough Neckline trough between the peaks.
 * @param[in] peak2 Second peak level.
 * @param[in] close Current close price.
 * @param[in] peak_tol Relative tolerance for matching the two peaks.
 * @return 1.0 when a double top is confirmed, otherwise 0.0.
 */
double cxta_ts_pattern_double_top(double peak1,
                               double trough,
                               double peak2,
                               double close,
                               double peak_tol);

/**
 * @brief Detect a confirmed double-bottom breakout.
 * @param[in] trough1 First trough level.
 * @param[in] peak Neckline peak between the troughs.
 * @param[in] trough2 Second trough level.
 * @param[in] close Current close price.
 * @param[in] trough_tol Relative tolerance for matching the two troughs.
 * @return 1.0 when a double bottom is confirmed, otherwise 0.0.
 */
double cxta_ts_pattern_double_bottom(double trough1,
                                  double peak,
                                  double trough2,
                                  double close,
                                  double trough_tol);

/**
 * @brief Detect a confirmed head-and-shoulders breakdown.
 * @param[in] left_shoulder Left shoulder level.
 * @param[in] head Head level.
 * @param[in] right_shoulder Right shoulder level.
 * @param[in] neckline Neckline break level.
 * @param[in] close Current close price.
 * @param[in] shoulder_tol Relative tolerance for matching shoulder heights.
 * @return 1.0 when the pattern is confirmed, otherwise 0.0.
 */
double cxta_ts_pattern_head_and_shoulders(double left_shoulder,
                                       double head,
                                       double right_shoulder,
                                       double neckline,
                                       double close,
                                       double shoulder_tol);

/**
 * @brief Detect a confirmed inverse head-and-shoulders breakout.
 * @param[in] left_shoulder_low Left shoulder low.
 * @param[in] head_low Head low.
 * @param[in] right_shoulder_low Right shoulder low.
 * @param[in] neckline Neckline break level.
 * @param[in] close Current close price.
 * @param[in] shoulder_tol Relative tolerance for matching shoulder lows.
 * @return 1.0 when the pattern is confirmed, otherwise 0.0.
 */
double cxta_ts_pattern_inverse_head_and_shoulders(double left_shoulder_low,
                                               double head_low,
                                               double right_shoulder_low,
                                               double neckline,
                                               double close,
                                               double shoulder_tol);

/**
 * @brief Detect a confirmed cup-and-handle breakout.
 * @param[in] left_peak Left rim peak.
 * @param[in] cup_low Lowest point of the cup.
 * @param[in] right_peak Right rim peak.
 * @param[in] handle_low Lowest point of the handle pullback.
 * @param[in] close Current close price.
 * @param[in] symmetry_tol Relative tolerance for matching the rim peaks.
 * @param[in] handle_max_ratio Maximum handle depth as a fraction of cup depth.
 * @return 1.0 when the pattern is confirmed, otherwise 0.0.
 */
double cxta_ts_pattern_cup_and_handle(double left_peak,
                                   double cup_low,
                                   double right_peak,
                                   double handle_low,
                                   double close,
                                   double symmetry_tol,
                                   double handle_max_ratio);

/**
 * @brief Detect a confirmed inverted cup-and-handle breakdown.
 * @param[in] left_low Left rim low.
 * @param[in] cup_high Highest point of the cup.
 * @param[in] right_low Right rim low.
 * @param[in] handle_high Highest point of the handle bounce.
 * @param[in] close Current close price.
 * @param[in] symmetry_tol Relative tolerance for matching the rim lows.
 * @param[in] handle_max_ratio Maximum handle depth as a fraction of cup depth.
 * @return 1.0 when the pattern is confirmed, otherwise 0.0.
 */
double cxta_ts_pattern_inverted_cup_and_handle(double left_low,
                                            double cup_high,
                                            double right_low,
                                            double handle_high,
                                            double close,
                                            double symmetry_tol,
                                            double handle_max_ratio);

#ifdef __cplusplus
}
#endif
