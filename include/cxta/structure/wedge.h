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
 * @brief Parsed broadening-pattern argument tuple.
 *
 * `breakout_source` is 0 for close and 1 for open. When
 * `has_trigger_value` is set, `trigger_value` overrides it and is used as
 * the current-bar breakout/breakdown source.
 */
typedef struct {
    int left;
    int right;
    int lookback;
    double min_width_ratio;
    int breakout_source;
    double trigger_value;
    int has_trigger_value;
    int auto_left_right;
} cxta_struct_broadening_args;

/**
 * @brief Broadening channel state projected at the current bar.
 */
typedef struct {
    double resistance;
    double support;
    double active_resistance;
    double active_support;
    double resistance_pivot;
    double resistance_pivot_index;
    double support_pivot;
    double support_pivot_index;
    double active_index;
    double width;
    double resistance_slope;
    double support_slope;
    double active;
    double rising;
    double falling;
    double breakout;
    double breakdown;
    double direction;
    double strength;
    int has_pivots;
} cxta_struct_broadening_state;

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

/**
 * @brief Parse `[left, right, lookback?, min_width_ratio?, breakout_source?]`.
 */
int cxta_struct_broadening_args_parse(const double* args,
                                      size_t nargs,
                                      size_t current_index,
                                      cxta_struct_broadening_args* out);

/**
 * @brief Compute broadening channel state for the current index/window.
 */
int cxta_struct_broadening_state_compute(const cxta_series_bar_view* view,
                                         const cxta_struct_broadening_args* args,
                                         cxta_struct_broadening_state* out);

/**
 * @brief Hide geometric fields when no active broadening pattern exists.
 */
void cxta_struct_broadening_state_mask_geometry(cxta_struct_broadening_state* out);

#ifdef __cplusplus
}
#endif
