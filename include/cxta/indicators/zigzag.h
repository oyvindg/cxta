/**
 * @file zigzag.h
 * @brief Causal ZigZag helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ZigZag output payload.
 */
typedef struct {
    double high;
    double low;
    double line;
    double pivot_index;
    double active;
    double active_index;
    double direction;
    double last;
    double is_high;
} cxta_zigzag_output;

/**
 * @brief Compute causal ZigZag outputs at the current view index.
 * @param[in] view Bar series view.
 * @param[in] threshold Reversal threshold as a fraction (e.g. `0.03` = 3%).
 * @param[in] n History index (`0` = newest pivot).
 * @return ZigZag outputs, or a zero-initialized struct when invalid.
 */
cxta_zigzag_output cxta_zigzag(const cxta_series_bar_view* view,
                               double threshold,
                               int n);

#ifdef __cplusplus
}
#endif
