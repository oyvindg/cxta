/**
 * @file signals.h
 * @brief Scalar signal helpers for expression runtimes.
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef double (*cxta_signal_pattern_fn)(const double* args, size_t nargs);

typedef enum cxta_signal_pattern_value_type {
    CXTA_SIGNAL_PATTERN_NUMBER = 0,
    CXTA_SIGNAL_PATTERN_BOOL = 1
} cxta_signal_pattern_value_type;

typedef struct cxta_signal_pattern_descriptor {
    const char* name;
    cxta_signal_pattern_fn fn;
    size_t min_args;
    size_t max_args;
    cxta_signal_pattern_value_type return_type;
} cxta_signal_pattern_descriptor;

/** @brief Return 1.0 when the first argument is strictly above the second. */
double cxta_signal_above(const double* args, size_t nargs);

/** @brief Return 1.0 when the first argument is strictly below the second. */
double cxta_signal_below(const double* args, size_t nargs);

/** @brief Normalize value into [0,1] for ascending or descending ranges. */
double cxta_signal_score(const double* args, size_t nargs);

/** @brief Return 1.0 when the first argument is strictly above the second. */
double cxta_signal_cross(const double* args, size_t nargs);

/** @brief Detect cross-above event from 2-arg or 4-arg call layouts. */
double cxta_signal_cross_above(const double* args, size_t nargs);

/** @brief Detect cross-below event from 2-arg or 4-arg call layouts. */
double cxta_signal_cross_below(const double* args, size_t nargs);

/** @brief Return cxta-owned shared signal and pattern descriptors. */
const cxta_signal_pattern_descriptor* cxta_signal_pattern_descriptors(size_t* out_count);

#ifdef __cplusplus
}
#endif
