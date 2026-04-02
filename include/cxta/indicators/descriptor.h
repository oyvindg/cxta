/**
 * @file descriptor.h
 * @brief Descriptor metadata for expression-facing indicators.
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>

#include <cxta/series/bar.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Sentinel offset used for scalar field aliases.
 *
 * This is used when an indicator exposes a field name that aliases the scalar
 * return value rather than a native output struct member.
 */
#define CXTA_FIELD_OFFSET_SCALAR ((size_t)-1)

/**
 * @brief Metadata for one exposed indicator field.
 */
typedef struct {
    const char* name;
    size_t offset;
    bool auto_plot;
} cxta_field_descriptor;

/**
 * @brief Capability flags for one indicator descriptor.
 */
typedef enum {
    CXTA_INDICATOR_SCALAR = 1u << 0,
    CXTA_INDICATOR_STRUCT = 1u << 1,
    CXTA_INDICATOR_SCALAR_SOURCE = 1u << 2,
    CXTA_INDICATOR_REPAINTING = 1u << 3,
} cxta_indicator_flags;

/**
 * @brief Full-scan scalar indicator entrypoint.
 */
typedef double (*cxta_scalar_fn)(const cxta_series_bar_view* view,
                                 const double* args,
                                 size_t nargs);

/**
 * @brief Full-scan struct indicator entrypoint.
 */
typedef void (*cxta_struct_fn)(const cxta_series_bar_view* view,
                               const double* args,
                               size_t nargs,
                               void* out);

/**
 * @brief Full-scan scalar-source entrypoint.
 */
typedef double (*cxta_scalar_src_fn)(const cxta_series_scalar_view* source,
                                     const double* args,
                                     size_t nargs);

/**
 * @brief Optional state-slot resolver for arg-dependent incremental state.
 *
 * Returns the number of `double` slots required for one state buffer.
 */
typedef size_t (*cxta_state_slots_fn)(const double* args,
                                      size_t nargs);

/**
 * @brief Optional stateful scalar step entrypoint.
 */
typedef double (*cxta_scalar_step_fn)(double close,
                                      double prev_close,
                                      const double* args,
                                      size_t nargs,
                                      void* state);

/**
 * @brief Optional stateful struct step entrypoint.
 */
typedef void (*cxta_struct_step_fn)(const cxta_series_bar_view* view,
                                    size_t index,
                                    const double* args,
                                    size_t nargs,
                                    void* state,
                                    void* out);

/**
 * @brief Metadata for one expression-facing indicator.
 */
typedef struct {
    const char* name;
    int min_args;
    int max_args;
    int scalar_source_min_args;
    int scalar_source_max_args;
    int primary_field_index;
    unsigned flags;
    size_t output_size;
    size_t state_size;
    const cxta_field_descriptor* fields;
    size_t field_count;
    cxta_scalar_fn eval_scalar;
    cxta_struct_fn eval_struct;
    cxta_scalar_src_fn eval_scalar_src;
    cxta_state_slots_fn state_slots;
    cxta_scalar_step_fn step_scalar;
    cxta_struct_step_fn step_struct;
} cxta_indicator_descriptor;

/**
 * @brief Return the descriptor inventory.
 * @param[out] count Optional descriptor count output.
 * @return Pointer to the static descriptor array.
 */
const cxta_indicator_descriptor* cxta_indicator_descriptors(size_t* count);

/**
 * @brief Find one descriptor by stable indicator name.
 * @param[in] name Stable expression-facing indicator name.
 * @return Matching descriptor, or `NULL` when not found.
 */
const cxta_indicator_descriptor* cxta_indicator_descriptor_find(const char* name);

#ifdef __cplusplus
}
#endif
