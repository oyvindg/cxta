/**
 * @file rolling.h
 * @brief Rolling mean/variance helpers.
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double sum;
    double sum_sq;
    size_t count;
} cxta_ts_rolling_state;

/**
 * @brief Reset a rolling accumulator to its empty state.
 * @param[in,out] st Rolling accumulator state.
 */
void cxta_ts_rolling_reset(cxta_ts_rolling_state* st);

/**
 * @brief Add a sample to a rolling accumulator.
 * @param[in,out] st Rolling accumulator state.
 * @param[in] x Sample value to add.
 */
void cxta_ts_rolling_add(cxta_ts_rolling_state* st, double x);

/**
 * @brief Remove a sample from a rolling accumulator.
 * @param[in,out] st Rolling accumulator state.
 * @param[in] x Sample value to remove.
 */
void cxta_ts_rolling_remove(cxta_ts_rolling_state* st, double x);

/**
 * @brief Compute the arithmetic mean of the accumulated samples.
 * @param[in] st Rolling accumulator state.
 * @return Mean value, or 0.0 when empty.
 */
double cxta_ts_rolling_mean(const cxta_ts_rolling_state* st);

/**
 * @brief Compute the population variance of the accumulated samples.
 * @param[in] st Rolling accumulator state.
 * @return Variance, or 0.0 when empty.
 */
double cxta_ts_rolling_var(const cxta_ts_rolling_state* st);

#ifdef __cplusplus
}
#endif
