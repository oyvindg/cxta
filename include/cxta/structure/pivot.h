/**
 * @file pivot.h
 * @brief Pivot helpers over bar views.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Parsed pivot argument tuple.
 */
typedef struct {
    int left;
    int right;
    int lookback;
} cxta_struct_pivot_args;

/**
 * @brief Pivot snapshot for current bar context.
 */
typedef struct {
    double high;
    double low;
    double is_high;
    double is_low;
    double line;
    double pivot_index;
    double prev_high;
    double prev_low;
    int has_prev_high;
    int has_prev_low;
    int has_last_high;
    int has_last_low;
} cxta_struct_pivot_state;

/**
 * @brief Parse `[left, right, lookback?]` pivot args.
 */
int cxta_struct_pivot_args_parse(const double* args, size_t nargs, size_t current_index, cxta_struct_pivot_args* out);
/**
 * @brief Check whether one index is a confirmed pivot high.
 */
int cxta_struct_pivot_is_high(const cxta_series_bar_view* view, size_t pivot_index, int left, int right);
/**
 * @brief Check whether one index is a confirmed pivot low.
 */
int cxta_struct_pivot_is_low(const cxta_series_bar_view* view, size_t pivot_index, int left, int right);
/**
 * @brief Compute pivot state for the current index/window.
 */
int cxta_struct_pivot_state_compute(const cxta_series_bar_view* view, const cxta_struct_pivot_args* args, cxta_struct_pivot_state* out);

#ifdef __cplusplus
}
#endif
