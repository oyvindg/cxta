/**
 * @file nvi.h
 * @brief Negative and Positive Volume Index helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Bridge metadata for NVI (no numeric parameters). */
extern const cxta_bridge_fn_spec cxta_nvi_bridge_fn_spec;

/** @brief Bridge metadata for PVI (no numeric parameters). */
extern const cxta_bridge_fn_spec cxta_pvi_bridge_fn_spec;

/** @brief Expression-facing descriptor for NVI. */
extern const cxta_indicator_descriptor cxta_nvi_descriptor;

/** @brief Expression-facing descriptor for PVI. */
extern const cxta_indicator_descriptor cxta_pvi_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Shared NVI/PVI state for incremental updates.
 */
typedef struct {
    double value;        /**< Current index value. */
    double prev_close;   /**< Previous close. */
    double prev_volume;  /**< Previous volume. */
    double initialized;  /**< Non-zero once seeded. */
} cxta_nvi_state;

/**
 * @brief Perform one NVI update.
 * @param[in] close Current close.
 * @param[in] volume Current volume.
 * @param[in,out] st NVI state.
 * @return Updated NVI value.
 */
double cxta_nvi_step(double close, double volume, cxta_nvi_state* st);

/**
 * @brief Perform one PVI update.
 * @param[in] close Current close.
 * @param[in] volume Current volume.
 * @param[in,out] st PVI state.
 * @return Updated PVI value.
 */
double cxta_pvi_step(double close, double volume, cxta_nvi_state* st);

/**
 * @brief Compute NVI at the current view index.
 * @param[in] view Bar series view.
 * @return NVI value, or 0.0 when invalid.
 */
double cxta_nvi(const cxta_series_bar_view* view);

/**
 * @brief Compute PVI at the current view index.
 * @param[in] view Bar series view.
 * @return PVI value, or 0.0 when invalid.
 */
double cxta_pvi(const cxta_series_bar_view* view);

#ifdef __cplusplus
}
#endif
