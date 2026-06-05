/**
 * @file types.h
 * @brief Public descriptor types for expression-facing indicators.
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

#ifdef __cplusplus
}
#endif
