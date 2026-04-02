/**
 * @file truerange.h
 * @brief True Range indicator wrapper.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute True Range for the current bar.
 *
 * Uses the previous bar's close when available; falls back to the bar's
 * own (H-L) range when at the first bar.
 *
 * @param[in] view Bar series view.
 * @return True Range value, or 0.0 when the view is invalid.
 */
double cxta_truerange(const cxta_series_bar_view* view);

#ifdef __cplusplus
}
#endif
