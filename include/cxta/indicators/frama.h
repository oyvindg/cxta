/**
 * @file frama.h
 * @brief FRAMA helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute FRAMA at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Fractal window period.
 * @return FRAMA value, or 0.0 when invalid.
 */
double cxta_frama(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
