/**
 * @file roc.h
 * @brief Rate of Change helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Rate of Change at the current view index.
 *
 * ROC = (close / close[n bars ago] - 1) * 100.
 *
 * @param[in] view   Bar series view.
 * @param[in] period Lookback period in bars.
 * @return ROC value in percent, or 0.0 when the view is invalid or the
 *         reference bar has a close of 0.
 */
double cxta_roc(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
