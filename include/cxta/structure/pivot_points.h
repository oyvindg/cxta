/**
 * @file pivot_points.h
 * @brief Classic floor-trader pivot points from previous UTC session OHLC.
 */

#pragma once

#include <cxta/series/bar.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Multi-output result for classic pivot points.
 */
typedef struct {
    double pp;
    double r1;
    double r2;
    double r3;
    double s1;
    double s2;
    double s3;
} cxta_struct_pivot_points_result;

/**
 * @brief Compute classic pivot points from the previous UTC session.
 *
 * Returns all-zero fields when the current bar has no completed previous
 * session to reference.
 */
int cxta_struct_pivot_points_compute(const cxta_series_bar_view* view,
                                     cxta_struct_pivot_points_result* out);

#ifdef __cplusplus
}
#endif
