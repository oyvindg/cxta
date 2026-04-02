/**
 * @file fib.h
 * @brief Fibonacci retracement and extension scalar predicates.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Price level at a Fibonacci retracement ratio.
 *
 * level = high - (high - low) * ratio = lerp(high, low, ratio).
 * ratio=0.0 → high, ratio=1.0 → low.
 * @param[in] high Swing high.
 * @param[in] low Swing low.
 * @param[in] ratio Retracement ratio to evaluate.
 * Returns 0.0 when high <= low.
 * @return Fibonacci retracement level.
 */
double cxta_ts_fib_level(double high, double low, double ratio);

/**
 * @brief Returns 1.0 when close is within tolerance of a Fibonacci level.
 *
 * Proximity is measured as a fraction of (high - low).
 * @param[in] high Swing high.
 * @param[in] low Swing low.
 * @param[in] close Current close price.
 * @param[in] ratio Retracement ratio to test.
 * @param[in] tolerance Allowed distance as a fraction of the swing range.
 * Returns 0.0 when high <= low.
 * @return 1.0 when close is near the requested Fibonacci level, otherwise 0.0.
 */
double cxta_ts_fib_near(double high, double low, double close,
                     double ratio, double tolerance);

/**
 * @brief Returns 1.0 when close is inside the Fibonacci band [ratio_lo, ratio_hi].
 *
 * ratio_lo < ratio_hi (e.g. 0.5 to 0.618 for the golden zone).
 * Band bounds are inclusive. Returns 0.0 when high <= low or ratio_lo >= ratio_hi.
 * @param[in] high Swing high.
 * @param[in] low Swing low.
 * @param[in] close Current close price.
 * @param[in] ratio_lo Upper retracement ratio of the band.
 * @param[in] ratio_hi Lower retracement ratio of the band.
 * @return 1.0 when close is inside the band, otherwise 0.0.
 */
double cxta_ts_fib_zone(double high, double low, double close,
                     double ratio_lo, double ratio_hi);

/**
 * @brief Fibonacci extension level projected from a retracement low.
 *
 * level = retrace_low + (swing_high - swing_low) * ratio.
 * @param[in] swing_low Swing low of the impulse leg.
 * @param[in] swing_high Swing high of the impulse leg.
 * @param[in] retrace_low Low point of the retracement.
 * @param[in] ratio Extension ratio to project.
 * Returns 0.0 when swing_high <= swing_low.
 * @return Fibonacci extension level.
 */
double cxta_ts_fib_extension(double swing_low, double swing_high,
                          double retrace_low, double ratio);

#ifdef __cplusplus
}
#endif
