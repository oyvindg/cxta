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
    &cxta_roc_descriptor,
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
    &cxta_roc_bridge_fn_spec,
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

const cxta_indicator_descriptor* cxta_indicator_descriptors(size_t* count) {
    static cxta_indicator_descriptor descriptors[CXTA_ARRAY_COUNT(kExternalDescriptors)];
    static int initialized = 0;
    size_t i;

    if (!initialized) {
        for (i = 0u; i < CXTA_ARRAY_COUNT(kExternalDescriptors); ++i) {
            descriptors[i] = *kExternalDescriptors[i];
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
    return descriptor != NULL &&
           field != NULL &&
           field->auto_plot &&
           (descriptor->flags & CXTA_INDICATOR_REPAINTING) == 0u;
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

int cxta_name_build_source_aware(const char* smoothing_name,
                                 const char* source_name,
                                 char* out,
                                 size_t out_size) {
    char suffix[256u];
    cxta_name_sanitize_suffix(source_name, suffix, sizeof(suffix));
    return snprintf(out, out_size, "%s_src_%s", smoothing_name, suffix);
}
