/**
 * @file wedge.h
 * @brief Wedge detection over confirmed pivot highs and lows.
 */

#pragma once

#include <stddef.h>

#include <cxta/series/bar.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Parsed wedge argument tuple.
 */
typedef struct {
    int left;
    int right;
    int lookback;
    double max_width_ratio;
    int auto_left_right;
} cxta_struct_wedge_args;

/**
 * @brief Wedge state projected at the current bar.
 */
typedef struct {
    double upper;
    double lower;
    double width;
    double upper_slope;
    double lower_slope;
    double active;
    double rising;
    double falling;
    double breakdown;
    double breakout;
    int has_pivots;
} cxta_struct_wedge_state;

/**
 * @brief Parse `[left, right, lookback?, max_width_ratio?]` wedge args.
 */
int cxta_struct_wedge_args_parse(const double* args,
                                 size_t nargs,
                                 size_t current_index,
                                 cxta_struct_wedge_args* out);

/**
 * @brief Compute wedge state for the current index/window.
 */
int cxta_struct_wedge_state_compute(const cxta_series_bar_view* view,
                                    const cxta_struct_wedge_args* args,
                                    cxta_struct_wedge_state* out);

/**
 * @brief Hide geometric fields when no active converging wedge exists.
 */
void cxta_struct_wedge_state_mask_geometry(cxta_struct_wedge_state* out);

#ifdef __cplusplus
}
#endif
