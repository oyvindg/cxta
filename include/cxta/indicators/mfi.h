/**
 * @file mfi.h
 * @brief Money Flow Index helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Money Flow Index at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return MFI in [0, 100], or 0.0 when invalid.
 */
double cxta_mfi(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
