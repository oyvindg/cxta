/**
 * @file aroon.h
 * @brief Aroon helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Aroon output payload.
 */
typedef struct {
    double up;         /**< Aroon Up in [0, 100]. */
    double down;       /**< Aroon Down in [0, 100]. */
    double oscillator; /**< Up - Down. */
} cxta_aroon_output;

/**
 * @brief Compute Aroon outputs at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return Aroon outputs, or a zero-initialized struct when the view is invalid.
 */
cxta_aroon_output cxta_aroon(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
