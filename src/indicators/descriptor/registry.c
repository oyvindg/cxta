/**
 * @file registry.c
 * @brief Descriptor and bridge-function registries for expression-facing indicators.
 */

#include <cxta/cxta.h>

#include <stddef.h>
#include <string.h>

static const cxta_param_descriptor cross_pair_params[] = {
    {"a"},
    {"b"},
};

static const cxta_param_descriptor cross_pair_period_params[] = {
    {"a"},
    {"b"},
    {"period"},
};

static const cxta_bridge_fn_spec spread_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("spread", 2u, 2u, cross_pair_params, 0);
static const cxta_bridge_fn_spec pair_spread_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("pair_spread", 2u, 2u, cross_pair_params, 0);
static const cxta_bridge_fn_spec covariance_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("covariance", 3u, 3u, cross_pair_period_params, 0);
static const cxta_bridge_fn_spec rolling_corr_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("rolling_corr", 3u, 3u, cross_pair_period_params, 0);
static const cxta_bridge_fn_spec rolling_beta_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("rolling_beta", 3u, 3u, cross_pair_period_params, 0);
static const cxta_bridge_fn_spec rolling_alpha_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("rolling_alpha", 3u, 3u, cross_pair_period_params, 0);
static const cxta_bridge_fn_spec hv_ratio_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("hv_ratio", 3u, 3u, cross_pair_period_params, 0);
static const cxta_bridge_fn_spec tracking_error_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("tracking_error", 3u, 3u, cross_pair_period_params, 0);
static const cxta_bridge_fn_spec relative_strength_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("relative_strength", 3u, 3u, cross_pair_period_params, 0);
static const cxta_bridge_fn_spec zscore_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("zscore", 3u, 3u, cross_pair_period_params, 0);

static const cxta_indicator_descriptor* const external_descriptors[] = {
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
    &cxta_broadening_descriptor,
};

static const cxta_bridge_fn_spec* const bridge_fn_specs[] = {
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
    &cxta_broadening_bridge_fn_spec,
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
    &spread_bridge_fn_spec,
    &pair_spread_bridge_fn_spec,
    &covariance_bridge_fn_spec,
    &rolling_corr_bridge_fn_spec,
    &rolling_beta_bridge_fn_spec,
    &rolling_alpha_bridge_fn_spec,
    &hv_ratio_bridge_fn_spec,
    &tracking_error_bridge_fn_spec,
    &relative_strength_bridge_fn_spec,
    &zscore_bridge_fn_spec,
};

static const char* cxta_descriptor_scalar_pane_for_name(const char* indicator_name) {
    (void)indicator_name;
    return "indicator";
}

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

const cxta_indicator_descriptor* cxta_indicator_descriptors(size_t* count) {
    static cxta_indicator_descriptor descriptors[CXTA_ARRAY_COUNT(external_descriptors)];
    static int initialized = 0;
    size_t i;

    if (!initialized) {
        for (i = 0u; i < CXTA_ARRAY_COUNT(external_descriptors); ++i) {
            descriptors[i] = *external_descriptors[i];
            descriptors[i].default_pane = cxta_descriptor_default_pane_for(&descriptors[i]);
        }
        initialized = 1;
    }
    if (count) {
        *count = CXTA_ARRAY_COUNT(external_descriptors);
    }
    return descriptors;
}

const cxta_bridge_fn_spec* const* cxta_bridge_fn_specs(size_t* count) {
    if (count) *count = CXTA_ARRAY_COUNT(bridge_fn_specs);
    return bridge_fn_specs;
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

static const char* const param_lr[] = {"left", "right"};

typedef struct {
    const char* indicator_name;
    cxta_param_spec spec;
} cxta_param_spec_entry;

static const cxta_param_spec_entry param_spec_table[] = {
    /* trendline: bridge + descriptor->params in trendline.h; table used for legacy name lookup */
    {"trendline", {param_lr, 2, NULL, -1}},
};

const cxta_param_spec* cxta_indicator_param_spec_find(const char* name) {
    size_t i;
    if (!name) return NULL;
    for (i = 0; i < sizeof(param_spec_table) / sizeof(param_spec_table[0]); ++i) {
        if (strcmp(param_spec_table[i].indicator_name, name) == 0)
            return &param_spec_table[i].spec;
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
