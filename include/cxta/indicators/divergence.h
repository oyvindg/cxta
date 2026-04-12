/**
 * @file divergence.h
 * @brief Two-series divergence helpers.
 */

#pragma once

#include "../series/bar.h"
#include "descriptor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Divergence output payload at the current scalar-series index.
 */
typedef struct {
    double bullish;
    double bearish;
    double bull_segment;
    double bear_segment;
} cxta_divergence_output;

/**
 * @brief Compute divergence outputs for one query index up to the current horizon.
 *
 * The views' `.index` values define the confirmation horizon. `query_index`
 * lets callers query the current bar or a lagged bar after later pivots have
 * already confirmed and backfilled the segment line.
 *
 * `bullish` and `bearish` fire on the confirmation bar. `bull_segment` and
 * `bear_segment` return the interpolated segment value at @p query_index when
 * that bar lies on a confirmed divergence segment, otherwise `NAN`.
 *
 * @param[in] primary Primary scalar series used for pivot detection.
 * @param[in] secondary Secondary scalar series compared against the pivots.
 * @param[in] left Pivot-left window.
 * @param[in] right Pivot-right window.
 * @param[in] lookback Maximum bars allowed between the pivot pair.
 * @param[in] query_index Absolute index to read after scanning to the current horizon.
 * @return Divergence outputs for @p query_index.
 */
cxta_divergence_output cxta_divergence_at(const cxta_series_scalar_view* primary,
                                          const cxta_series_scalar_view* secondary,
                                          int left,
                                          int right,
                                          int lookback,
                                          size_t query_index);

/**
 * @brief Convenience wrapper for divergence outputs at the current index.
 */
cxta_divergence_output cxta_divergence(const cxta_series_scalar_view* primary,
                                       const cxta_series_scalar_view* secondary,
                                       int left,
                                       int right,
                                       int lookback);

/** @brief Expression-facing descriptor for divergence (evaluated via bridge sources). */
extern const cxta_indicator_descriptor cxta_divergence_descriptor;
/** @brief Bridge metadata for source-aware divergence calls. */
extern const cxta_bridge_fn_spec cxta_divergence_bridge_fn_spec;

#ifdef __cplusplus
}
#endif
