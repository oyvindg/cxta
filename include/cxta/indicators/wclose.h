/**
 * @file wclose.h
 * @brief Weighted close price helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute weighted close (H+L+2C)/4 for the current bar.
 * @param[in] view Bar series view.
 * @return Weighted close, or 0.0 when the view is invalid.
 */
double cxta_wclose(const cxta_series_bar_view* view);

#ifdef __cplusplus
}
#endif
