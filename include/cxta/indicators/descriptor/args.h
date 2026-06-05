/**
 * @file args.h
 * @brief Descriptor argument helpers for expression-facing indicators.
 */

#pragma once

#include <stddef.h>

#include <cxta/indicators/descriptor/types.h>
#include <cxta/structure/pivot.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Copy one struct output payload into the evaluator output buffer.
 * @param[out] out Destination buffer.
 * @param[in] value Source struct address.
 * @param[in] size Copy size in bytes.
 */
void cxta_descriptor_copy_struct(void* out, const void* value, size_t size);

/**
 * @brief Parse swing/pivot numeric args using the current bar index as context.
 * @param[in] view Bar series view (used for current index when resolving relative pivots).
 * @param[in] args Positional numeric arguments from the expression call.
 * @param[in] nargs Length of @p args.
 * @param[out] out Parsed pivot arguments.
 * @return Non-zero when parsing succeeds.
 */
int cxta_descriptor_parse_pivot_args(const cxta_series_bar_view* view,
                                     const double* args,
                                     size_t nargs,
                                     cxta_struct_pivot_args* out);

/**
 * @brief Read one integer argument from a numeric argument list, with fallback.
 * @param[in] args Argument array, or NULL.
 * @param[in] nargs Number of entries in @p args.
 * @param[in] index Zero-based index of the argument to read.
 * @param[in] fallback Value used when @p args is NULL, @p index is out of range, or the value is non-finite.
 * @return Rounded integer in `[INT_MIN, INT_MAX]`, or @p fallback when unavailable.
 */
int cxta_descriptor_int_arg(const double* args, size_t nargs, size_t index, int fallback);

/**
 * @brief Read one period-style integer argument, clamped via `cxta_ts_clamp_period`.
 * @param[in] args Argument array, or NULL.
 * @param[in] nargs Number of entries in @p args.
 * @param[in] index Zero-based index of the argument to read.
 * @param[in] fallback Value passed to `cxta_descriptor_int_arg` when the slot is missing or invalid.
 * @return Clamped positive period suitable for indicator windows.
 */
int cxta_descriptor_period_arg(const double* args, size_t nargs, size_t index, int fallback);

/**
 * @brief Read one floating-point argument from a numeric argument list, with fallback.
 * @param[in] args Argument array, or NULL.
 * @param[in] nargs Number of entries in @p args.
 * @param[in] index Zero-based index of the argument to read.
 * @param[in] fallback Value used when @p args is NULL, @p index is out of range, or the value is non-finite.
 * @return Finite double, or @p fallback.
 */
double cxta_descriptor_double_arg(const double* args, size_t nargs, size_t index, double fallback);

/**
 * @brief Read one integer argument and clamp it to an inclusive range.
 * @param[in] args Argument array, or NULL.
 * @param[in] nargs Number of entries in @p args.
 * @param[in] index Zero-based index of the argument to read.
 * @param[in] fallback Base value from `cxta_descriptor_int_arg` before clamping.
 * @param[in] min_value Minimum allowed result (inclusive).
 * @param[in] max_value Maximum allowed result (inclusive).
 * @return Clamped integer in `[min_value, max_value]`.
 */
int cxta_descriptor_clamp_int_arg(const double* args,
                                  size_t nargs,
                                  size_t index,
                                  int fallback,
                                  int min_value,
                                  int max_value);

#ifdef __cplusplus
}
#endif
