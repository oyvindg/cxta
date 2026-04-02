/**
 * @file coppock.h
 * @brief Coppock Curve helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Coppock Curve at the current view index.
 * @param[in] view Bar series view.
 * @param[in] wma_period WMA period over the ROC sum.
 * @param[in] long_roc Long ROC period.
 * @param[in] short_roc Short ROC period.
 * @return Coppock Curve value, or 0.0 when invalid.
 */
double cxta_coppock(const cxta_series_bar_view* view,
                    int wma_period,
                    int long_roc,
                    int short_roc);

#ifdef __cplusplus
}
#endif
