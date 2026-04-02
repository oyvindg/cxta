/**
 * @file extrema.h
 * @brief Rolling extrema helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Rolling extrema output.
 */
typedef struct {
    double high; /**< Highest high over the window. */
    double low;  /**< Lowest low over the window. */
    double mid;  /**< Midpoint (high + low) / 2. */
} cxta_extrema_output;

/**
 * @brief Compute rolling high/low/mid over a bar window.
 * @param[in] view   Bar series view.
 * @param[in] period Lookback period.
 * @return Rolling extrema, or zero-initialized struct when the view is invalid.
 */
cxta_extrema_output cxta_extrema(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
