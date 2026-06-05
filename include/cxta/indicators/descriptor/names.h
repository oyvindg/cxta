/**
 * @file names.h
 * @brief Descriptor name helpers for generated function names.
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Sanitize a suffix so it is safe in generated function names.
 * @param[in] name Input suffix.
 * @param[out] out Target buffer for the sanitized string.
 * @param[in] out_size Size of `out` in bytes.
 */
void cxta_name_sanitize_suffix(const char* name, char* out, size_t out_size);

/**
 * @brief Build the generated timeframe function name for one base name.
 * @param[in] name Base indicator or field name.
 * @param[out] out Target buffer for the generated name.
 * @param[in] out_size Size of `out` in bytes.
 * @return `snprintf`-style written length.
 */
int cxta_name_build_timeframe(const char* name, char* out, size_t out_size);

#ifdef __cplusplus
}
#endif
