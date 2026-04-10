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
 * @brief Named parameter descriptor for one indicator argument.
 *
 * Array index matches the corresponding positional argument index.
 */
typedef struct {
    const char* name;
} cxta_param_descriptor;

/**
 * @brief Expression-visible argument category for bridge-level rewrites.
 */
typedef enum {
    CXTA_EXPR_ARG_NUMERIC = 0,
    CXTA_EXPR_ARG_SCALAR_SOURCE = 1,
} cxta_expr_arg_kind;

/**
 * @brief Expression-level argument descriptor for source-aware calls.
 */
typedef struct {
    const char* name;
    cxta_expr_arg_kind kind;
    const char* default_value;
} cxta_expr_arg_descriptor;

/**
 * @brief Bridge-facing function-signature metadata exported by cxta modules.
 *
 * This keeps expression-visible parameter naming close to the indicator module
 * itself without forcing bridge-specific code into `cxpr-bridge`.
 */
typedef struct {
    const char* name;                     /**< Expression-visible function name. */
    size_t min_args;                      /**< Minimum accepted argument count. */
    size_t max_args;                      /**< Maximum accepted argument count. */
    const cxta_param_descriptor* params;  /**< Ordered parameter descriptors, or NULL. */
    size_t param_count;                   /**< Number of entries in @p params. */
    const cxta_expr_arg_descriptor* expr_args; /**< Expression args, including source args, or NULL. */
    size_t expr_arg_count;                /**< Number of entries in @p expr_args. */
    int has_optional_timeframe_param;     /**< Non-zero when a trailing optional timeframe is accepted. */
} cxta_bridge_fn_spec;

/** @brief Return the number of elements in a fixed-size array. */
#define CXTA_ARRAY_COUNT(values) (sizeof(values) / sizeof((values)[0]))

/**
 * @brief Build one bridge-facing function spec from a fixed-size param array.
 * @param name_literal Expression-visible function name.
 * @param min_arg_count Minimum accepted argument count.
 * @param max_arg_count Maximum accepted argument count.
 * @param params_array Fixed-size `cxta_param_descriptor` array.
 * @param optional_timeframe_flag Non-zero when a trailing optional timeframe is accepted.
 */
#define CXTA_BRIDGE_FN_SPEC(name_literal, min_arg_count, max_arg_count, params_array, optional_timeframe_flag)     {                                                                                                                  (name_literal),                                                                                                (min_arg_count),                                                                                               (max_arg_count),                                                                                               (params_array),                                                                                                CXTA_ARRAY_COUNT(params_array),                                                                                NULL,                                                                                                          0u,                                                                                                            (optional_timeframe_flag)                                                                                  }

/**
 * @brief Build one bridge-facing function spec with expression-level arg metadata.
 */
#define CXTA_BRIDGE_FN_SPEC_EXPR(name_literal, min_arg_count, max_arg_count, params_array, expr_args_array, optional_timeframe_flag)     {                                                                                                                                     (name_literal),                                                                                                                   (min_arg_count),                                                                                                                  (max_arg_count),                                                                                                                  (params_array),                                                                                                                   CXTA_ARRAY_COUNT(params_array),                                                                                                   (expr_args_array),                                                                                                                CXTA_ARRAY_COUNT(expr_args_array),                                                                                                (optional_timeframe_flag)                                                                                                     }

/**
 * @brief Capability flags for one indicator descriptor.
 */
typedef enum {
    CXTA_INDICATOR_SCALAR = 1u << 0,        /**< Supports scalar output via `eval_scalar` or `step_scalar`. */
    CXTA_INDICATOR_STRUCT = 1u << 1,        /**< Supports structured output via `eval_struct` or `step_struct`. */
    CXTA_INDICATOR_SCALAR_SOURCE = 1u << 2, /**< Supports scalar-source input via `eval_scalar_src`. */
    CXTA_INDICATOR_REPAINTING = 1u << 3,    /**< May change previously emitted values as new bars arrive. */
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
    const char* name;                     /**< Stable expression-facing indicator name. */
    int min_args;                         /**< Minimum accepted numeric argument count. */
    int max_args;                         /**< Maximum accepted numeric argument count. */
    int scalar_source_min_args;           /**< Minimum args for source-aware scalar forms, or -1 when unsupported. */
    int scalar_source_max_args;           /**< Maximum args for source-aware scalar forms, or -1 when unsupported. */
    int primary_field_index;              /**< Preferred output field index for plotting/default selection, or -1. */
    unsigned flags;                       /**< Indicator capability flags. */
    size_t output_size;                   /**< Size in bytes of the scalar/struct output payload. */
    size_t state_size;                    /**< Fixed incremental state size in bytes, or 0 when stateless/dynamic. */
    const cxta_field_descriptor* fields;  /**< Output field descriptors for struct-capable indicators, or NULL. */
    size_t field_count;                   /**< Number of entries in fields. */
    cxta_scalar_fn eval_scalar;           /**< Scalar evaluation entrypoint, or NULL. */
    cxta_struct_fn eval_struct;           /**< Struct evaluation entrypoint, or NULL. */
    cxta_scalar_src_fn eval_scalar_src;   /**< Source-aware scalar evaluation entrypoint, or NULL. */
    cxta_state_slots_fn state_slots;      /**< Dynamic state-slot resolver, or NULL. */
    cxta_scalar_step_fn step_scalar;      /**< Incremental scalar step entrypoint, or NULL. */
    cxta_struct_step_fn step_struct;      /**< Incremental struct step entrypoint, or NULL. */
    const cxta_param_descriptor* params;  /**< Named parameter descriptors, or NULL. */
    size_t param_count;                   /**< Number of entries in params. */
} cxta_indicator_descriptor;

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

/**
 * @brief Return whether one field should be auto-plotted by the host.
 * @param[in] descriptor Owning descriptor.
 * @param[in] field Field metadata to inspect.
 * @return `true` when the field should be auto-plotted.
 */
bool cxta_indicator_field_auto_plot(const cxta_indicator_descriptor* descriptor,
                                    const cxta_field_descriptor* field);

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

/**
 * @brief Build the generated source-aware smoothing function name.
 * @param[in] smoothing_name Consumer indicator name.
 * @param[in] source_name Source indicator or field name.
 * @param[out] out Target buffer for the generated name.
 * @param[in] out_size Size of `out` in bytes.
 * @return `snprintf`-style written length.
 */
int cxta_name_build_source_aware(const char* smoothing_name,
                                 const char* source_name,
                                 char* out,
                                 size_t out_size);

#ifdef __cplusplus
}
#endif
