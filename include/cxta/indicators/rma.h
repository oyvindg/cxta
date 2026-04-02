/**
 * @file rma.h
 * @brief Wilder RMA helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief RMA state for incremental updates.
 */
typedef struct {
    double value;       /**< Current RMA value. */
    double initialized; /**< Non-zero once seeded. */
} cxta_rma_state;

/**
 * @brief Perform one RMA update.
 * @param[in] sample Current sample.
 * @param[in] period RMA period.
 * @param[in,out] st RMA state.
 * @return Updated RMA value.
 */
double cxta_rma_step(double sample, int period, cxta_rma_state* st);

/**
 * @brief Compute close-based RMA at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period RMA period.
 * @return RMA value, or 0.0 when the view is invalid.
 */
double cxta_rma(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
