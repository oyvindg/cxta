/**
 * @file mama.h
 * @brief MAMA/FAMA helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MAMA output payload.
 */
typedef struct {
    double mama; /**< Adaptive moving average. */
    double fama; /**< Following adaptive moving average. */
} cxta_mama_output;

/**
 * @brief Compute MAMA/FAMA outputs at the current view index.
 * @param[in] view Bar series view.
 * @param[in] fast_limit Fast alpha limit.
 * @param[in] slow_limit Slow alpha limit.
 * @return MAMA outputs, or a zero-initialized struct when invalid.
 */
cxta_mama_output cxta_mama(const cxta_series_bar_view* view,
                           double fast_limit,
                           double slow_limit);

#ifdef __cplusplus
}
#endif
