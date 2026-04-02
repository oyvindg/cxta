/**
 * @file bollinger.h
 * @brief Bollinger band helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Bollinger output payload.
 */
typedef struct {
    double upper;
    double lower;
    double middle;
    double bandwidth;
    double percent_b;
} cxta_bollinger_output;

/**
 * @brief Compute Bollinger bands at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @param[in] stddev_mult Standard-deviation multiplier.
 * @return Bollinger outputs.
 */
cxta_bollinger_output cxta_bollinger(const cxta_series_bar_view* view,
                                     int period,
                                     double stddev_mult);

#ifdef __cplusplus
}
#endif
