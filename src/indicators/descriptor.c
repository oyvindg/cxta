/**
 * @file descriptor.c
 * @brief Descriptor metadata for expression-facing indicators.
 */

#include <cxta/cxta.h>

#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

int cxta_descriptor_int_arg(const double* args,
                                   size_t nargs,
                                   size_t index,
                                   int fallback) {
    double raw;

    if (!args || index >= nargs) return fallback;
    raw = args[index];
    if (!isfinite(raw)) return fallback;
    if (raw >= (double)INT_MAX) return INT_MAX;
    if (raw <= (double)INT_MIN) return INT_MIN;
    return (int)llround(raw);
}

int cxta_descriptor_period_arg(const double* args,
                                      size_t nargs,
                                      size_t index,
                                      int fallback) {
    return cxta_ts_clamp_period(cxta_descriptor_int_arg(args, nargs, index, fallback));
}

int cxta_descriptor_clamp_int_arg(const double* args,
                                         size_t nargs,
                                         size_t index,
                                         int fallback,
                                         int min_value,
                                         int max_value) {
    int value = cxta_descriptor_int_arg(args, nargs, index, fallback);
    if (value < min_value) value = min_value;
    if (value > max_value) value = max_value;
    return value;
}

double cxta_descriptor_double_arg(const double* args,
                                         size_t nargs,
                                         size_t index,
                                         double fallback) {
    double raw;

    if (!args || index >= nargs) return fallback;
    raw = args[index];
    return isfinite(raw) ? raw : fallback;
}

int cxta_descriptor_parse_pivot_args(const cxta_series_bar_view* view,
                                            const double* args,
                                            size_t nargs,
                                            cxta_struct_pivot_args* out) {
    const size_t current_index =
        (view && view->size > 0u) ? cxta_series_clamp_index(view->size, view->index) : 0u;
    return cxta_struct_pivot_args_parse(args, nargs, current_index, out);
}

void cxta_descriptor_copy_struct(void* out, const void* value, size_t size) {
    if (!out || !value || size == 0u) return;
    memcpy(out, value, size);
}

static const cxta_param_descriptor kCrossPairParams[] = {
    {"a"},
    {"b"},
};

static const cxta_param_descriptor kCrossPairPeriodParams[] = {
    {"a"},
    {"b"},
    {"period"},
};

static const cxta_bridge_fn_spec kSpreadBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("spread", 2u, 2u, kCrossPairParams, 0);
static const cxta_bridge_fn_spec kPairSpreadBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("pair_spread", 2u, 2u, kCrossPairParams, 0);
static const cxta_bridge_fn_spec kCovarianceBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("covariance", 3u, 3u, kCrossPairPeriodParams, 0);
static const cxta_bridge_fn_spec kRollingCorrBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("rolling_corr", 3u, 3u, kCrossPairPeriodParams, 0);
static const cxta_bridge_fn_spec kRollingBetaBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("rolling_beta", 3u, 3u, kCrossPairPeriodParams, 0);
static const cxta_bridge_fn_spec kRollingAlphaBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("rolling_alpha", 3u, 3u, kCrossPairPeriodParams, 0);
static const cxta_bridge_fn_spec kHvRatioBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("hv_ratio", 3u, 3u, kCrossPairPeriodParams, 0);
static const cxta_bridge_fn_spec kTrackingErrorBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("tracking_error", 3u, 3u, kCrossPairPeriodParams, 0);
static const cxta_bridge_fn_spec kRelativeStrengthBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("relative_strength", 3u, 3u, kCrossPairPeriodParams, 0);
static const cxta_bridge_fn_spec kZScoreBridgeFnSpec =
    CXTA_BRIDGE_FN_SPEC("z_score", 3u, 3u, kCrossPairPeriodParams, 0);

static const cxta_indicator_descriptor* const kExternalDescriptors[] = {
    &cxta_supertrend_descriptor,
    &cxta_dema_descriptor,
    &cxta_tema_descriptor,
    &cxta_hma_descriptor,
    &cxta_linear_regression_slope_descriptor,
    &cxta_linreg_angle_descriptor,
    &cxta_historical_volatility_descriptor,
    &cxta_realized_volatility_descriptor,
    &cxta_true_range_descriptor,
    &cxta_typical_price_descriptor,
    &cxta_median_price_descriptor,
    &cxta_weighted_close_descriptor,
    &cxta_obv_descriptor,
    &cxta_awesome_oscillator_descriptor,
    &cxta_bop_descriptor,
    &cxta_ease_of_movement_descriptor,
    &cxta_cmf_descriptor,
    &cxta_cmo_descriptor,
    &cxta_kst_descriptor,
    &cxta_mass_index_descriptor,
    &cxta_fisher_transform_descriptor,
    &cxta_crsi_descriptor,
    &cxta_coppock_curve_descriptor,
    &cxta_schaff_trend_cycle_descriptor,
    &cxta_choppiness_index_descriptor,
    &cxta_dominant_cycle_period_descriptor,
    &cxta_parabolic_sar_descriptor,
    &cxta_anchored_vwap_descriptor,
    &cxta_macd_descriptor,
    &cxta_rsi_descriptor,
    &cxta_ema_descriptor,
    &cxta_atr_descriptor,
    &cxta_sma_descriptor,
    &cxta_rma_descriptor,
    &cxta_wma_descriptor,
    &cxta_stddev_descriptor,
    &cxta_zscore_descriptor,
    &cxta_cci_descriptor,
    &cxta_dpo_descriptor,
    &cxta_frama_descriptor,
    &cxta_kama_descriptor,
    &cxta_mfi_descriptor,
    &cxta_rvi_descriptor,
    &cxta_trix_descriptor,
    &cxta_ulcer_index_descriptor,
    &cxta_vidya_descriptor,
    &cxta_williams_r_descriptor,
    &cxta_bollinger_descriptor,
    &cxta_zigzag_descriptor,
    &cxta_fair_value_gap_descriptor,
    &cxta_highest_descriptor,
    &cxta_lowest_descriptor,
    &cxta_rolling_max_descriptor,
    &cxta_rolling_min_descriptor,
    &cxta_rolling_max_close_descriptor,
    &cxta_rolling_min_close_descriptor,
    &cxta_chaikin_oscillator_descriptor,
    &cxta_ppo_descriptor,
    &cxta_tsi_descriptor,
    &cxta_elder_force_descriptor,
    &cxta_nvi_descriptor,
    &cxta_pvi_descriptor,
    &cxta_adx_descriptor,
    &cxta_donchian_descriptor,
    &cxta_keltner_descriptor,
    &cxta_price_channel_descriptor,
    &cxta_aroon_descriptor,
    &cxta_stochastic_descriptor,
    &cxta_stoch_rsi_descriptor,
    &cxta_mama_descriptor,
    &cxta_pvo_descriptor,
    &cxta_ichimoku_descriptor,
    &cxta_vortex_descriptor,
    &cxta_vwap_descriptor,
    &cxta_ttm_squeeze_descriptor,
    &cxta_trendline_descriptor,
    &cxta_volume_profile_descriptor,
    &cxta_pivot_points_descriptor,
    &cxta_swing_pivots_descriptor,
    &cxta_structure_descriptor,
    &cxta_bos_descriptor,
    &cxta_fvg_descriptor,
    &cxta_order_block_descriptor,
    &cxta_liquidity_descriptor,
    &cxta_liquidity_pools_descriptor,
    &cxta_sfp_descriptor,
    &cxta_swing_anchor_vwap_descriptor,
    &cxta_wedge_descriptor,
    &cxta_divergence_descriptor,
};

static const cxta_bridge_fn_spec* const kBridgeFnSpecs[] = {
    &cxta_macd_bridge_fn_spec,
    &cxta_rsi_bridge_fn_spec,
    &cxta_ema_bridge_fn_spec,
    &cxta_atr_bridge_fn_spec,
    &cxta_sma_bridge_fn_spec,
    &cxta_rma_bridge_fn_spec,
    &cxta_wma_bridge_fn_spec,
    &cxta_stddev_bridge_fn_spec,
    &cxta_zscore_bridge_fn_spec,
    &cxta_cci_bridge_fn_spec,
    &cxta_dpo_bridge_fn_spec,
    &cxta_frama_bridge_fn_spec,
    &cxta_kama_bridge_fn_spec,
    &cxta_mfi_bridge_fn_spec,
    &cxta_rvi_bridge_fn_spec,
    &cxta_trix_bridge_fn_spec,
    &cxta_ulcer_index_bridge_fn_spec,
    &cxta_vidya_bridge_fn_spec,
    &cxta_williams_r_bridge_fn_spec,
    &cxta_rolling_max_bridge_fn_spec,
    &cxta_rolling_min_bridge_fn_spec,
    &cxta_rolling_max_close_bridge_fn_spec,
    &cxta_rolling_min_close_bridge_fn_spec,
    &cxta_chaikin_oscillator_bridge_fn_spec,
    &cxta_ppo_bridge_fn_spec,
    &cxta_tsi_bridge_fn_spec,
    &cxta_elder_force_bridge_fn_spec,
    &cxta_nvi_bridge_fn_spec,
    &cxta_pvi_bridge_fn_spec,
    &cxta_adx_bridge_fn_spec,
    &cxta_donchian_bridge_fn_spec,
    &cxta_keltner_bridge_fn_spec,
    &cxta_price_channel_bridge_fn_spec,
    &cxta_aroon_bridge_fn_spec,
    &cxta_stochastic_bridge_fn_spec,
    &cxta_stoch_rsi_bridge_fn_spec,
    &cxta_mama_bridge_fn_spec,
    &cxta_pvo_bridge_fn_spec,
    &cxta_ichimoku_bridge_fn_spec,
    &cxta_vortex_bridge_fn_spec,
    &cxta_vwap_bridge_fn_spec,
    &cxta_ttm_squeeze_bridge_fn_spec,
    &cxta_trendline_bridge_fn_spec,
    &cxta_volume_profile_bridge_fn_spec,
    &cxta_bollinger_bridge_fn_spec,
    &cxta_zigzag_bridge_fn_spec,
    &cxta_fair_value_gap_bridge_fn_spec,
    &cxta_highest_bridge_fn_spec,
    &cxta_lowest_bridge_fn_spec,
    &cxta_pivot_points_bridge_fn_spec,
    &cxta_swing_pivots_bridge_fn_spec,
    &cxta_structure_bridge_fn_spec,
    &cxta_bos_bridge_fn_spec,
    &cxta_fvg_bridge_fn_spec,
    &cxta_order_block_bridge_fn_spec,
    &cxta_liquidity_bridge_fn_spec,
    &cxta_liquidity_pools_bridge_fn_spec,
    &cxta_sfp_bridge_fn_spec,
    &cxta_swing_anchor_vwap_bridge_fn_spec,
    &cxta_wedge_bridge_fn_spec,
    &cxta_divergence_bridge_fn_spec,
    &cxta_supertrend_bridge_fn_spec,
    &cxta_dema_bridge_fn_spec,
    &cxta_tema_bridge_fn_spec,
    &cxta_hma_bridge_fn_spec,
    &cxta_linear_regression_slope_bridge_fn_spec,
    &cxta_linreg_angle_bridge_fn_spec,
    &cxta_historical_volatility_bridge_fn_spec,
    &cxta_realized_volatility_bridge_fn_spec,
    &cxta_true_range_bridge_fn_spec,
    &cxta_typical_price_bridge_fn_spec,
    &cxta_median_price_bridge_fn_spec,
    &cxta_weighted_close_bridge_fn_spec,
    &cxta_obv_bridge_fn_spec,
    &cxta_awesome_oscillator_bridge_fn_spec,
    &cxta_bop_bridge_fn_spec,
    &cxta_ease_of_movement_bridge_fn_spec,
    &cxta_cmf_bridge_fn_spec,
    &cxta_cmo_bridge_fn_spec,
    &cxta_kst_bridge_fn_spec,
    &cxta_mass_index_bridge_fn_spec,
    &cxta_fisher_transform_bridge_fn_spec,
    &cxta_crsi_bridge_fn_spec,
    &cxta_coppock_curve_bridge_fn_spec,
    &cxta_schaff_trend_cycle_bridge_fn_spec,
    &cxta_choppiness_index_bridge_fn_spec,
    &cxta_dominant_cycle_period_bridge_fn_spec,
    &cxta_parabolic_sar_bridge_fn_spec,
    &cxta_anchored_vwap_bridge_fn_spec,
    &kSpreadBridgeFnSpec,
    &kPairSpreadBridgeFnSpec,
    &kCovarianceBridgeFnSpec,
    &kRollingCorrBridgeFnSpec,
    &kRollingBetaBridgeFnSpec,
    &kRollingAlphaBridgeFnSpec,
    &kHvRatioBridgeFnSpec,
    &kTrackingErrorBridgeFnSpec,
    &kRelativeStrengthBridgeFnSpec,
    &kZScoreBridgeFnSpec,
};

typedef struct {
    const char* name;
    cxta_scalar_plot_descriptor plot;
} cxta_builtin_plot_entry;

static const cxta_builtin_plot_entry kBuiltinPlotDescriptors[] = {
    {
        "open",
        {true, "Open", "price", "#94a3b8", "line", "price", NULL, NULL},
    },
    {
        "high",
        {true, "High", "price", "#22c55e", "line", "price", NULL, NULL},
    },
    {
        "low",
        {true, "Low", "price", "#ef4444", "line", "price", NULL, NULL},
    },
    {
        "close",
        {true, "Close", "price", "#93c5fd", "line", "price", NULL, NULL},
    },
    {
        "volume",
        {true, "Volume", "volume", "#94a3b8", "histogram", "volume", "#22c55e", "#ef4444"},
    },
};

const cxta_scalar_plot_descriptor* cxta_builtin_plot_descriptor_find(
    const char* name) {
    size_t i;

    if (!name || name[0] == '\0') return NULL;
    for (i = 0u; i < CXTA_ARRAY_COUNT(kBuiltinPlotDescriptors); ++i) {
        if (strcmp(kBuiltinPlotDescriptors[i].name, name) == 0) {
            return &kBuiltinPlotDescriptors[i].plot;
        }
    }
    return NULL;
}

static const char* cxta_descriptor_default_pane_for(
    const cxta_indicator_descriptor* descriptor);

static const char* cxta_descriptor_scalar_pane_for_name(const char* indicator_name) {
    (void)indicator_name;
    return "indicator";
}

static int cxta_descriptor_name_is_acronym(const char* name, size_t start, size_t end) {
    static const char* const acronyms[] = {
        "adx", "ao", "atr", "bop", "cci", "cmf", "cmo", "crsi", "dpo", "ema",
        "fvg", "hma", "kst", "mfi", "nvi", "ppo", "pvi", "rma", "rsi",
        "rvi", "sfp", "sma", "tsi", "vwap", "wma",
    };
    char token[32];
    size_t i;
    size_t n = end > start ? end - start : 0u;

    if (!name || n == 0u || n >= sizeof(token)) return 0;
    for (i = 0u; i < n; ++i) token[i] = name[start + i];
    token[n] = '\0';
    for (i = 0u; i < CXTA_ARRAY_COUNT(acronyms); ++i) {
        if (strcmp(token, acronyms[i]) == 0) return 1;
    }
    return 0;
}

static void cxta_descriptor_make_label(const char* name, char* out, size_t out_size) {
    size_t i = 0u;
    size_t o = 0u;
    size_t word_start = 0u;

    if (!out || out_size == 0u) return;
    out[0] = '\0';
    if (!name || name[0] == '\0') return;

    while (name[i] != '\0' && o + 1u < out_size) {
        while (name[i] == '_') ++i;
        if (name[i] == '\0') break;
        word_start = i;
        while (name[i] != '\0' && name[i] != '_') ++i;

        if (o > 0u && o + 1u < out_size) out[o++] = ' ';
        if (cxta_descriptor_name_is_acronym(name, word_start, i)) {
            size_t j;
            for (j = word_start; j < i && o + 1u < out_size; ++j) {
                char c = name[j];
                out[o++] = (char)((c >= 'a' && c <= 'z') ? (c - 'a' + 'A') : c);
            }
        } else {
            size_t j;
            for (j = word_start; j < i && o + 1u < out_size; ++j) {
                char c = name[j];
                if (j == word_start && c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
                out[o++] = c;
            }
        }
    }
    out[o] = '\0';
}

static void cxta_descriptor_make_scalar_hover(const char* indicator_name,
                                              char* summary,
                                              size_t summary_size,
                                              char* indication,
                                              size_t indication_size) {
    char label[96];
    cxta_descriptor_make_label(indicator_name, label, sizeof(label));
    if (summary && summary_size > 0u) {
        snprintf(summary, summary_size, "%s indicator output.", label[0] ? label : "Scalar");
    }
    if (!indication || indication_size == 0u) return;
    snprintf(indication, indication_size,
             "Generic fallback metadata; prefer an indicator-owned plot descriptor for semantic chart behavior.");
}

static void cxta_descriptor_make_field_hover(const char* indicator_name,
                                             const char* field_name,
                                             char* summary,
                                             size_t summary_size,
                                             char* indication,
                                             size_t indication_size) {
    char indicator_label[96];
    char field_label[96];
    cxta_descriptor_make_label(indicator_name, indicator_label, sizeof(indicator_label));
    cxta_descriptor_make_label(field_name, field_label, sizeof(field_label));
    if (summary && summary_size > 0u) {
        snprintf(summary, summary_size, "%s %s output field.",
                 indicator_label[0] ? indicator_label : "Indicator",
                 field_label[0] ? field_label : "value");
    }
    if (!indication || indication_size == 0u) return;
    if (strcmp(field_name ? field_name : "", "histogram") == 0) {
        snprintf(indication, indication_size,
                 "Histogram field; sign and bar expansion show spread direction and momentum.");
    } else if (strcmp(field_name ? field_name : "", "upper") == 0 ||
               strcmp(field_name ? field_name : "", "lower") == 0 ||
               strcmp(field_name ? field_name : "", "middle") == 0) {
        snprintf(indication, indication_size,
                 "Band/channel field; use distance from price and touches or breaks for context.");
    } else if (strstr(field_name ? field_name : "", "signal") != NULL ||
               strstr(field_name ? field_name : "", "break") != NULL ||
               strstr(field_name ? field_name : "", "direction") != NULL) {
        snprintf(indication, indication_size,
                 "Signal-style field; prefer transitions and confirmations over isolated values.");
    } else {
        snprintf(indication, indication_size,
                 "Plot with related fields from the same indicator to read trend, spread, or structure.");
    }
}

const cxta_indicator_descriptor* cxta_indicator_descriptors(size_t* count) {
    static cxta_indicator_descriptor descriptors[CXTA_ARRAY_COUNT(kExternalDescriptors)];
    static int initialized = 0;
    size_t i;

    if (!initialized) {
        for (i = 0u; i < CXTA_ARRAY_COUNT(kExternalDescriptors); ++i) {
            descriptors[i] = *kExternalDescriptors[i];
            descriptors[i].default_pane = cxta_descriptor_default_pane_for(&descriptors[i]);
        }
        initialized = 1;
    }
    if (count) {
        *count = CXTA_ARRAY_COUNT(kExternalDescriptors);
    }
    return descriptors;
}

const cxta_bridge_fn_spec* const* cxta_bridge_fn_specs(size_t* count) {
    if (count) *count = CXTA_ARRAY_COUNT(kBridgeFnSpecs);
    return kBridgeFnSpecs;
}

const cxta_bridge_fn_spec* cxta_bridge_fn_spec_find(const char* name) {
    size_t count = 0u;
    const cxta_bridge_fn_spec* const* specs = cxta_bridge_fn_specs(&count);
    size_t i;

    if (!name) return NULL;
    for (i = 0u; i < count; ++i) {
        const cxta_bridge_fn_spec* spec = specs[i];
        if (spec && spec->name && strcmp(spec->name, name) == 0) return spec;
    }
    return NULL;
}

const cxta_indicator_descriptor* cxta_indicator_descriptor_find(const char* name) {
    size_t count = 0;
    const cxta_indicator_descriptor* descriptors = cxta_indicator_descriptors(&count);
    size_t i;

    if (!name) return NULL;

    for (i = 0; i < count; ++i) {
        if (strcmp(descriptors[i].name, name) == 0) return &descriptors[i];
    }
    return NULL;
}

/* -------------------------------------------------------------------------- */
/* Named parameter specs                                                       */
/* -------------------------------------------------------------------------- */

static const char* const kParamLR[] = {"left", "right"};

typedef struct {
    const char* indicator_name;
    cxta_param_spec spec;
} cxta_param_spec_entry;

static const cxta_param_spec_entry kParamSpecTable[] = {
    /* trendline: bridge + descriptor->params in trendline.h; table used for legacy name lookup */
    {"trendline", {kParamLR, 2, NULL, -1}},
};

static const char* cxta_descriptor_default_pane_for(
    const cxta_indicator_descriptor* descriptor) {
    const cxta_indicator_plot_descriptor* plot_descriptor;

    if (!descriptor || !descriptor->name) return NULL;
    if (descriptor->default_pane && descriptor->default_pane[0] != '\0') {
        return descriptor->default_pane;
    }

    plot_descriptor = descriptor->plot;
    if (plot_descriptor && plot_descriptor->scalar && plot_descriptor->scalar->pane &&
        plot_descriptor->scalar->pane[0] != '\0') {
        return plot_descriptor->scalar->pane;
    }
    if (plot_descriptor && plot_descriptor->fields && plot_descriptor->field_count > 0u &&
        plot_descriptor->fields[0].pane &&
        plot_descriptor->fields[0].pane[0] != '\0') {
        return plot_descriptor->fields[0].pane;
    }
    return cxta_descriptor_scalar_pane_for_name(descriptor->name);
}

const cxta_param_spec* cxta_indicator_param_spec_find(const char* name) {
    size_t i;
    if (!name) return NULL;
    for (i = 0; i < sizeof(kParamSpecTable) / sizeof(kParamSpecTable[0]); ++i) {
        if (strcmp(kParamSpecTable[i].indicator_name, name) == 0)
            return &kParamSpecTable[i].spec;
    }
    return NULL;
}

bool cxta_indicator_descriptor_supports_scalar_source(
    const cxta_indicator_descriptor* descriptor) {
    return descriptor != NULL &&
           (descriptor->flags & CXTA_INDICATOR_SCALAR_SOURCE) != 0u &&
           descriptor->scalar_source_min_args >= 0 &&
           descriptor->scalar_source_max_args >= 0;
}

bool cxta_indicator_field_auto_plot(const cxta_indicator_descriptor* descriptor,
                                    const cxta_field_descriptor* field) {
    const cxta_plot_field_descriptor* plot_field;

    if (!descriptor || !field) return false;
    plot_field = cxta_indicator_plot_field_descriptor_find(descriptor->name, field->name);
    return descriptor != NULL &&
           field != NULL &&
           (plot_field ? plot_field->auto_plot : field->auto_plot) &&
           (descriptor->flags & CXTA_INDICATOR_REPAINTING) == 0u;
}

const cxta_indicator_plot_descriptor* cxta_indicator_plot_descriptor_find(
    const char* indicator_name) {
    static cxta_scalar_plot_descriptor scalar_fallback;
    static cxta_indicator_plot_descriptor indicator_fallback;
    static char scalar_label[96];
    static char scalar_hover_summary[160];
    static char scalar_hover_indication[192];
    const cxta_indicator_descriptor* indicator_descriptor;

    if (!indicator_name || indicator_name[0] == '\0') return NULL;

    indicator_descriptor = cxta_indicator_descriptor_find(indicator_name);
    if (indicator_descriptor && indicator_descriptor->plot) {
        return indicator_descriptor->plot;
    }
    if (!indicator_descriptor || indicator_descriptor->field_count != 0u) return NULL;

    cxta_descriptor_make_label(indicator_name, scalar_label, sizeof(scalar_label));
    cxta_descriptor_make_scalar_hover(indicator_name,
                                      scalar_hover_summary,
                                      sizeof(scalar_hover_summary),
                                      scalar_hover_indication,
                                      sizeof(scalar_hover_indication));
    memset(&scalar_fallback, 0, sizeof(scalar_fallback));
    scalar_fallback.auto_plot = true;
    scalar_fallback.label = scalar_label;
    scalar_fallback.pane =
        (indicator_descriptor->default_pane && indicator_descriptor->default_pane[0] != '\0')
            ? indicator_descriptor->default_pane
            : cxta_descriptor_scalar_pane_for_name(indicator_name);
    scalar_fallback.color = "#93c5fd";
    scalar_fallback.style = "line";
    scalar_fallback.scale = scalar_fallback.pane;
    scalar_fallback.hover_summary = scalar_hover_summary;
    scalar_fallback.hover_indication = scalar_hover_indication;

    memset(&indicator_fallback, 0, sizeof(indicator_fallback));
    indicator_fallback.indicator_name = indicator_name;
    indicator_fallback.scalar = &scalar_fallback;
    return &indicator_fallback;
}

static const char* cxta_descriptor_field_color_fallback(
    const cxta_field_descriptor* field) {
    if (!field || !field->name) return "#93c5fd";
    if (strcmp(field->name, "histogram") == 0) return "#a855f7";
    if (strstr(field->name, "plus") != NULL ||
        strstr(field->name, "bull") != NULL ||
        strcmp(field->name, "up") == 0 ||
        strcmp(field->name, "upper") == 0) {
        return "#22c55e";
    }
    if (strstr(field->name, "minus") != NULL ||
        strstr(field->name, "bear") != NULL ||
        strcmp(field->name, "down") == 0 ||
        strcmp(field->name, "lower") == 0) {
        return "#ef4444";
    }
    if (strcmp(field->name, "signal") == 0) return "#f97316";
    if (strcmp(field->name, "line") == 0) return "#22d3ee";
    if (strcmp(field->name, "value") == 0) return "#38bdf8";
    return "#93c5fd";
}

const cxta_scalar_plot_descriptor* cxta_indicator_scalar_plot_descriptor_find(
    const char* indicator_name) {
    static cxta_scalar_plot_descriptor merged;
    static char hover_summary[160];
    static char hover_indication[192];
    const cxta_indicator_plot_descriptor* descriptor =
        cxta_indicator_plot_descriptor_find(indicator_name);
    const cxta_scalar_plot_descriptor* scalar;
    const char* pane;

    if (!descriptor) return NULL;
    scalar = descriptor->scalar;
    if (!scalar) return NULL;

    merged = *scalar;
    pane = merged.pane && merged.pane[0] != '\0'
               ? merged.pane
               : cxta_descriptor_scalar_pane_for_name(indicator_name);
    if (!merged.pane || merged.pane[0] == '\0') merged.pane = pane;
    if (!merged.scale || merged.scale[0] == '\0') merged.scale = pane;
    if (!merged.color || merged.color[0] == '\0') merged.color = "#93c5fd";
    if (!merged.style || merged.style[0] == '\0') merged.style = "line";
    if (!merged.hover_summary || merged.hover_summary[0] == '\0' ||
        !merged.hover_indication || merged.hover_indication[0] == '\0') {
        cxta_descriptor_make_scalar_hover(indicator_name,
                                          hover_summary,
                                          sizeof(hover_summary),
                                          hover_indication,
                                          sizeof(hover_indication));
        if (!merged.hover_summary || merged.hover_summary[0] == '\0') {
            merged.hover_summary = hover_summary;
        }
        if (!merged.hover_indication || merged.hover_indication[0] == '\0') {
            merged.hover_indication = hover_indication;
        }
    }
    return &merged;
}

const cxta_plot_field_descriptor* cxta_indicator_plot_field_descriptor_find(
    const char* indicator_name,
    const char* field_name) {
    static cxta_plot_field_descriptor fallbacks[16];
    static char field_labels[16][96];
    static char field_hover_summaries[16][192];
    static char field_hover_indications[16][224];
    static size_t fallback_index = 0u;
    cxta_plot_field_descriptor* fallback;
    char* field_label;
    char* field_hover_summary;
    char* field_hover_indication;
    const cxta_indicator_plot_descriptor* descriptor;
    const cxta_indicator_descriptor* indicator_descriptor;
    const cxta_field_descriptor* field_descriptor = NULL;
    size_t i;

    if (!field_name || field_name[0] == '\0') return NULL;
    fallback = &fallbacks[fallback_index % CXTA_ARRAY_COUNT(fallbacks)];
    field_label = field_labels[fallback_index % CXTA_ARRAY_COUNT(field_labels)];
    field_hover_summary =
        field_hover_summaries[fallback_index % CXTA_ARRAY_COUNT(field_hover_summaries)];
    field_hover_indication =
        field_hover_indications[fallback_index % CXTA_ARRAY_COUNT(field_hover_indications)];
    fallback_index++;

    indicator_descriptor = cxta_indicator_descriptor_find(indicator_name);
    if (indicator_descriptor && indicator_descriptor->fields) {
        for (i = 0u; i < indicator_descriptor->field_count; ++i) {
            if (indicator_descriptor->fields[i].name &&
                strcmp(indicator_descriptor->fields[i].name, field_name) == 0) {
                field_descriptor = &indicator_descriptor->fields[i];
                break;
            }
        }
    }

    descriptor = cxta_indicator_plot_descriptor_find(indicator_name);
    if (descriptor && descriptor->fields) {
        for (i = 0u; i < descriptor->field_count; ++i) {
            const cxta_plot_field_descriptor* field = &descriptor->fields[i];
            if (!field->field_name) continue;
            if (strcmp(field->field_name, field_name) == 0) {
                *fallback = *field;
                cxta_descriptor_make_label(field_name, field_label, 96u);
                cxta_descriptor_make_field_hover(indicator_name,
                                                field_name,
                                                field_hover_summary,
                                                192u,
                                                field_hover_indication,
                                                224u);
                if (!fallback->label || fallback->label[0] == '\0') fallback->label = field_label;
                if (!fallback->pane || fallback->pane[0] == '\0') {
                    fallback->pane =
                        (indicator_descriptor && indicator_descriptor->default_pane &&
                         indicator_descriptor->default_pane[0] != '\0')
                            ? indicator_descriptor->default_pane
                            : cxta_descriptor_scalar_pane_for_name(indicator_name);
                }
                if (!fallback->color || fallback->color[0] == '\0') {
                    fallback->color = cxta_descriptor_field_color_fallback(field_descriptor);
                }
                if (!fallback->style || fallback->style[0] == '\0') {
                    fallback->style = strcmp(field_name, "histogram") == 0 ? "histogram" : "line";
                }
                if (!fallback->scale || fallback->scale[0] == '\0') fallback->scale = fallback->pane;
                if (!fallback->hover_summary || fallback->hover_summary[0] == '\0') {
                    fallback->hover_summary = field_hover_summary;
                }
                if (!fallback->hover_indication || fallback->hover_indication[0] == '\0') {
                    fallback->hover_indication = field_hover_indication;
                }
                return fallback;
            }
        }
    }

    if (!field_descriptor) return NULL;

    memset(fallback, 0, sizeof(*fallback));
    fallback->field_name = field_name;
    fallback->auto_plot = field_descriptor->auto_plot;
    cxta_descriptor_make_label(field_name, field_label, 96u);
    fallback->label = field_label;
    fallback->pane =
        (indicator_descriptor->default_pane && indicator_descriptor->default_pane[0] != '\0')
            ? indicator_descriptor->default_pane
            : cxta_descriptor_scalar_pane_for_name(indicator_name);
    fallback->color = cxta_descriptor_field_color_fallback(field_descriptor);
    fallback->style = strcmp(field_name, "histogram") == 0 ? "histogram" : "line";
    fallback->scale = fallback->pane;
    if (strcmp(field_name, "histogram") == 0) {
        fallback->positive_color = "#22c55e";
        fallback->negative_color = "#ef4444";
    }
    cxta_descriptor_make_field_hover(indicator_name,
                                    field_name,
                                    field_hover_summary,
                                    192u,
                                    field_hover_indication,
                                    224u);
    fallback->hover_summary = field_hover_summary;
    fallback->hover_indication = field_hover_indication;
    return fallback;
}

void cxta_name_sanitize_suffix(const char* name, char* out, size_t out_size) {
    size_t i;

    if (!out || out_size == 0u) return;
    out[0] = '\0';
    if (!name) return;

    for (i = 0u; name[i] != '\0' && i + 1u < out_size; ++i) {
        const char c = name[i];
        const int is_ident =
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '_';
        out[i] = is_ident ? c : '_';
    }
    out[i] = '\0';
}

int cxta_name_build_timeframe(const char* name, char* out, size_t out_size) {
    char suffix[256u];
    cxta_name_sanitize_suffix(name, suffix, sizeof(suffix));
    return snprintf(out, out_size, "%s_tf", suffix);
}
