/**
 * @file eom.h
 * @brief Ease of Movement helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Ease of Movement for the current bar.
 *
 * EOM = ((H+L)/2 − prev(H+L)/2) × (H−L) / volume
 *
 * Returns 0.0 at the first bar, or when volume or bar range is zero.
 *
 * @param[in] view Bar series view.
 * @return EOM value, or 0.0 when the view is invalid or first bar.
 */
double cxta_eom(const cxta_series_bar_view* view);

#ifdef __cplusplus
}
#endif
