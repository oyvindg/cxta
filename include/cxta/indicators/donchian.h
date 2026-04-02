/**
 * @file donchian.h
 * @brief Donchian channel helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Generic channel output payload.
 */
typedef struct {
    double upper;
    double lower;
    double middle;
    double width;
} cxta_channel_output;

/**
 * @brief Compute a Donchian channel at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return Channel outputs.
 */
cxta_channel_output cxta_donchian(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif