/**
 * @file descriptor.h
 * @brief Descriptor metadata for expression-facing indicators.
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>

#include <cxta/series/bar.h>
#include <cxta/structure/pivot.h>

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
    const char* name;   /**< Stable field name for expressions and plotting. */
    size_t offset;      /**< Byte offset into the output struct, or `CXTA_FIELD_OFFSET_SCALAR`. */
    bool auto_plot;     /**< When true, hosts may include this field in default charts. */
} cxta_field_descriptor;

/**
 * @brief Optional GUI plot metadata for one exposed indicator field.
 *
 * Hosts may use this to pre-populate plot specs instead of relying on
 * indicator-name heuristics for pane, color, and style selection.
 */
typedef struct {
    const char* field_name;       /**< Stable output field name. */
    bool auto_plot;               /**< When true, hosts may include this field in default charts. */
    const char* label;            /**< Default chart label, or NULL. */
    const char* pane;             /**< Default pane name, or NULL. */
    const char* color;            /**< Primary series color, or NULL. */
    const char* style;            /**< Plot style ("line", "histogram", ...), or NULL. */
    const char* scale;            /**< Optional scale name, or NULL. */
    const char* positive_color;   /**< Optional positive histogram color, or NULL. */
    const char* negative_color;   /**< Optional negative histogram color, or NULL. */
    const char* pivot_value_field;  /**< Optional sparse-geometry pivot value field, or NULL. */
    const char* pivot_index_field;  /**< Optional sparse-geometry pivot index field, or NULL. */
    const char* active_value_field; /**< Optional sparse-geometry active value field, or NULL. */
    const char* active_index_field; /**< Optional sparse-geometry active index field, or NULL. */
    bool pivot_zero_is_missing;     /**< Treat zero pivot value as missing geometry. */
    bool active_zero_is_missing;    /**< Treat zero active value as missing geometry. */
    const char* hover_summary;      /**< Optional concise hover summary, or NULL. */
    const char* hover_indication;   /**< Optional usage guidance for editor hover, or NULL. */
    bool show_price;                /**< When true, hosts may draw price candles in this field's pane. */
} cxta_plot_field_descriptor;

/**
 * @brief Optional GUI plot metadata for one scalar indicator output.
 */
typedef struct {
    bool auto_plot;               /**< When true, hosts may include this indicator in default charts. */
    const char* label;            /**< Default chart label, or NULL. */
    const char* pane;             /**< Default pane name, or NULL. */
    const char* color;            /**< Primary series color, or NULL. */
    const char* style;            /**< Plot style ("line", "histogram", ...), or NULL. */
    const char* scale;            /**< Optional scale name, or NULL. */
    const char* positive_color;   /**< Optional positive histogram color, or NULL. */
    const char* negative_color;   /**< Optional negative histogram color, or NULL. */
    const char* hover_summary;    /**< Optional concise hover summary, or NULL. */
    const char* hover_indication; /**< Optional usage guidance for editor hover, or NULL. */
} cxta_scalar_plot_descriptor;

/**
 * @brief Find optional GUI plot metadata for one built-in market-data series.
 * @param[in] name Stable series name such as "close" or "volume".
 * @return Matching scalar plot metadata, or `NULL` when none is defined.
 */
const cxta_scalar_plot_descriptor* cxta_builtin_plot_descriptor_find(
    const char* name);

/**
 * @brief Optional GUI plot metadata for one indicator family.
 */
typedef struct {
    const char* indicator_name;                     /**< Stable indicator name. */
    const cxta_scalar_plot_descriptor* scalar;      /**< Optional scalar output metadata. */
    const cxta_plot_field_descriptor* fields;       /**< Optional per-field metadata. */
    size_t field_count;                             /**< Number of entries in @p fields. */
} cxta_indicator_plot_descriptor;

/**
 * @brief Named parameter descriptor for one indicator argument.
 *
 * Array index matches the corresponding positional argument index.
 */
typedef struct {
    const char* name;   /**< Stable parameter name for bridge/codegen. */
} cxta_param_descriptor;

/**
 * @brief Expression-visible argument category for bridge-level rewrites.
 */
typedef enum {
    CXTA_EXPR_ARG_NUMERIC = 0,        ///< Ordinary numeric parameter.
    CXTA_EXPR_ARG_SCALAR_SOURCE = 1, ///< Series or scalar source binding.
} cxta_expr_arg_kind;

/**
 * @brief Expression-level argument descriptor for source-aware calls.
 */
typedef struct {
    const char* name;          /**< Argument name for diagnostics and bridge metadata. */
    cxta_expr_arg_kind kind;    /**< Numeric vs scalar-source argument. */
    const char* default_value; /**< Default string when optional, or NULL. */
    const char* hover_summary; /**< Optional concise hover summary, or NULL. */
} cxta_expr_arg_descriptor;

/**
 * @brief Bridge-facing function-signature metadata exported by cxta modules.
 *
 * This keeps expression-visible parameter naming close to the indicator module
 * itself without forcing bridge-specific code into `cxpr`.
 *
 * @note Use `CXTA_BRIDGE_FN_SPEC_EXPR` when a function has multiple optional
 *       positional parameters and hosts need **partial named-argument** rewrite
 *       (default strings per slot). `CXTA_BRIDGE_FN_SPEC` alone supplies names
 *       but not defaults; without `expr_args`, adapters cannot fill omitted
 *       leading parameters for named-only tails.
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
 *
 * @note For optional multi-argument functions that use named-arg sugar with
 *       omitted leading parameters, prefer `CXTA_BRIDGE_FN_SPEC_EXPR` with
 *       defaults aligned to the indicator parse path.
 */
#define CXTA_BRIDGE_FN_SPEC(name_literal, min_arg_count, max_arg_count, params_array, optional_timeframe_flag)     {                                                                                                                  (name_literal),                                                                                                (min_arg_count),                                                                                               (max_arg_count),                                                                                               (params_array),                                                                                                CXTA_ARRAY_COUNT(params_array),                                                                                NULL,                                                                                                          0u,                                                                                                            (optional_timeframe_flag)                                                                                  }

/**
 * @brief Build one bridge-facing function spec with expression-level arg metadata.
 * @param name_literal Expression-visible function name.
 * @param min_arg_count Minimum accepted argument count.
 * @param max_arg_count Maximum accepted argument count.
 * @param params_array Fixed-size `cxta_param_descriptor` array.
 * @param expr_args_array Fixed-size `cxta_expr_arg_descriptor` array.
 * @param optional_timeframe_flag Non-zero when a trailing optional timeframe is accepted.
 */
#define CXTA_BRIDGE_FN_SPEC_EXPR(name_literal, min_arg_count, max_arg_count, params_array, expr_args_array, optional_timeframe_flag)     {                                                                                                                                     (name_literal),                                                                                                                   (min_arg_count),                                                                                                                  (max_arg_count),                                                                                                                  (params_array),                                                                                                                   CXTA_ARRAY_COUNT(params_array),                                                                                                   (expr_args_array),                                                                                                                CXTA_ARRAY_COUNT(expr_args_array),                                                                                                (optional_timeframe_flag)                                                                                                     }

/**
 * @brief Capability flags for one indicator descriptor.
 */
typedef enum {
    CXTA_INDICATOR_SCALAR = 1u << 0,        ///< Supports scalar output via `eval_scalar` or `step_scalar`.
    CXTA_INDICATOR_STRUCT = 1u << 1,        ///< Supports structured output via `eval_struct` or `step_struct`.
    CXTA_INDICATOR_SCALAR_SOURCE = 1u << 2, ///< Supports scalar-source input via `eval_scalar_src`.
    CXTA_INDICATOR_REPAINTING = 1u << 3,    ///< May change previously emitted values as new bars arrive.
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
    const char* default_pane;             /**< Default chart pane, or NULL to let hosts fall back to "price". */
    const cxta_indicator_plot_descriptor* plot; /**< Optional indicator-owned plot metadata. */
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
 * @brief Find optional GUI plot metadata for one indicator.
 * @param[in] indicator_name Stable expression-facing indicator name.
 * @return Matching plot descriptor, or `NULL` when none is defined.
 */
const cxta_indicator_plot_descriptor* cxta_indicator_plot_descriptor_find(
    const char* indicator_name);

/**
 * @brief Find optional GUI plot metadata for one scalar indicator output.
 * @param[in] indicator_name Stable expression-facing indicator name.
 * @return Matching scalar plot descriptor, or `NULL` when none is defined.
 */
const cxta_scalar_plot_descriptor* cxta_indicator_scalar_plot_descriptor_find(
    const char* indicator_name);

/**
 * @brief Find optional GUI plot metadata for one indicator field.
 * @param[in] indicator_name Stable expression-facing indicator name.
 * @param[in] field_name Stable output field name.
 * @return Matching field plot descriptor, or `NULL` when none is defined.
 */
const cxta_plot_field_descriptor* cxta_indicator_plot_field_descriptor_find(
    const char* indicator_name,
    const char* field_name);

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
