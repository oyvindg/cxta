/**
 * @file vortex.h
 * @brief Vortex helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Vortex output payload.
 */
typedef struct {
    double vi_plus;  /**< Positive vortex line. */
    double vi_minus; /**< Negative vortex line. */
    double trend;    /**< `vi_plus - vi_minus`. */
} cxta_vortex_output;

/**
 * @brief Compute Vortex outputs at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return Vortex outputs, or a zero-initialized struct when invalid.
 */
cxta_vortex_output cxta_vortex(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
