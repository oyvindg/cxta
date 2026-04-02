/**
 * @file kst.h
 * @brief KST helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute KST at the current view index.
 * @param[in] view Bar series view.
 * @param[in] p1 First ROC period.
 * @param[in] p2 Second ROC period.
 * @param[in] p3 Third ROC period.
 * @param[in] p4 Fourth ROC period.
 * @return KST value, or 0.0 when invalid.
 */
double cxta_kst(const cxta_series_bar_view* view, int p1, int p2, int p3, int p4);

#ifdef __cplusplus
}
#endif
