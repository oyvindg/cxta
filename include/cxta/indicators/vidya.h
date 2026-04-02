/**
 * @file vidya.h
 * @brief VIDYA helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute VIDYA at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period VIDYA base period.
 * @return VIDYA value, or 0.0 when invalid.
 */
double cxta_vidya(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
