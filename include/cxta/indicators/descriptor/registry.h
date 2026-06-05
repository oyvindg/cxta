/**
 * @file registry.h
 * @brief Descriptor and bridge-function registry lookup helpers.
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>

#include <cxta/indicators/descriptor/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return the descriptor inventory.
 * @param[out] count Optional descriptor count output.
 * @return Pointer to the static descriptor array.
 */
const cxta_indicator_descriptor* cxta_indicator_descriptors(size_t* count);

/**
 * @brief Return the bridge-facing function-spec inventory exported by cxta.
 * @param[out] count Optional function-spec count output.
 * @return Pointer to the static function-spec pointer array.
 */
const cxta_bridge_fn_spec* const* cxta_bridge_fn_specs(size_t* count);

/**
 * @brief Find one bridge-facing function spec by stable function name.
 * @param[in] name Stable expression-facing function name.
 * @return Matching bridge spec, or `NULL` when not found.
 */
const cxta_bridge_fn_spec* cxta_bridge_fn_spec_find(const char* name);

/**
 * @brief Find one descriptor by stable indicator name.
 * @param[in] name Stable expression-facing indicator name.
 * @return Matching descriptor, or `NULL` when not found.
 */
const cxta_indicator_descriptor* cxta_indicator_descriptor_find(const char* name);

/**
 * @brief Named parameter metadata for one indicator's numeric arguments.
 *
 * Covers only numeric (non-expression-source) parameters. Expression-level
 * source arguments (such as series inputs to divergence) are described at
 * the bridge layer.
 */
typedef struct {
    const char* const* names;    /**< Ordered parameter names (never NULL when count > 0). */
    size_t count;                /**< Number of entries in names. */
    const char* const* defaults; /**< Parallel default strings (NULL entry = no default), or NULL when all required. */
    int min_count;               /**< -1 means same as count (all required); >= 0 overrides. */
} cxta_param_spec;

/**
 * @brief Look up named parameter metadata for one indicator.
 * @param[in] name Stable indicator name.
 * @return Pointer to static spec, or NULL when the indicator has no named parameters.
 */
const cxta_param_spec* cxta_indicator_param_spec_find(const char* name);

/**
 * @brief Return whether a descriptor supports scalar-source invocation.
 * @param[in] descriptor Descriptor to inspect.
 * @return `true` when the descriptor supports scalar-source forms.
 */
bool cxta_indicator_descriptor_supports_scalar_source(
    const cxta_indicator_descriptor* descriptor);

#ifdef __cplusplus
}
#endif
