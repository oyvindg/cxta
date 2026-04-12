/**
 * @file vwap.h
 * @brief Swing-anchor VWAP helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Parsed args for swing-anchor adaptive VWAP.
 */
typedef struct {
    int swing_period;
    double apt;
    int use_adapt;
    double vol_bias;
    int atr_period;
    double min_apt;
    double max_apt;
} cxta_struct_vwap_args;

/**
 * @name swing_anchor_vwap default parameters
 *
 * Single source of truth for `cxta_struct_vwap_args_parse` when no arguments
 * are passed, and for `CXTA_BRIDGE_FN_SPEC_EXPR` default strings consumed by
 * cxpr-bridge / strategy named-arg rewrite. Keep numeric and `_STR` pairs aligned.
 *
 * @{
 */
#define CXTA_STRUCT_VWAP_DEFAULT_SWING_PERIOD 50
#define CXTA_STRUCT_VWAP_DEFAULT_APT 20.0
#define CXTA_STRUCT_VWAP_DEFAULT_USE_ADAPT 0
#define CXTA_STRUCT_VWAP_DEFAULT_VOL_BIAS 10.0
#define CXTA_STRUCT_VWAP_DEFAULT_ATR_PERIOD 50
#define CXTA_STRUCT_VWAP_DEFAULT_MIN_APT 5.0
#define CXTA_STRUCT_VWAP_DEFAULT_MAX_APT 300.0

#define CXTA_STRUCT_VWAP_DEFAULT_SWING_PERIOD_STR "50"
#define CXTA_STRUCT_VWAP_DEFAULT_APT_STR "20"
#define CXTA_STRUCT_VWAP_DEFAULT_USE_ADAPT_STR "0"
#define CXTA_STRUCT_VWAP_DEFAULT_VOL_BIAS_STR "10"
#define CXTA_STRUCT_VWAP_DEFAULT_ATR_PERIOD_STR "50"
#define CXTA_STRUCT_VWAP_DEFAULT_MIN_APT_STR "5"
#define CXTA_STRUCT_VWAP_DEFAULT_MAX_APT_STR "300"
/** @} */

/**
 * @brief Multi-output result for swing-anchor adaptive VWAP.
 */
typedef struct {
    double value;
    double direction;
    double reanchor;
    double anchor_price;
    double anchor_bars;
    double hh;
    double hl;
    double lh;
    double ll;
    double last_high;
    double last_low;
    double bull_vwap;   ///< VWAP anchored from most recent swing low
    double bear_vwap;   ///< VWAP anchored from most recent swing high
} cxta_struct_vwap_result;

typedef struct {
    double atr;
    double atr_avg;
    double last_high;
    double last_low;
    size_t last_high_index;
    size_t last_low_index;
    double prev_ref;
    double prev_high_state;
    double prev_low_state;
    int have_prev_ref;
    double prev_direction;
    size_t anchor_index;
    double anchor_price;
    double p_acc;
    double v_acc;
    double last_high_atr;
    double last_high_atr_avg;
    double last_low_atr;
    double last_low_atr_avg;
    int have_anchor;
    double bull_p_acc;
    double bull_v_acc;
    int bull_have_anchor;
    double bear_p_acc;
    double bear_v_acc;
    int bear_have_anchor;
} cxta_struct_vwap_state;

/**
 * @brief Parse `[swing_period, apt, use_adapt, vol_bias, atr_period, min_apt, max_apt]`.
 */
int cxta_struct_vwap_args_parse(const double* args,
                                     size_t nargs,
                                     cxta_struct_vwap_args* out);
/**
 * @brief Compute rolling swing-anchor adaptive VWAP at the current bar.
 */
int cxta_struct_vwap_compute(const cxta_series_bar_view* view,
                                  const cxta_struct_vwap_args* args,
                                  cxta_struct_vwap_result* out);

void cxta_struct_vwap_state_reset(cxta_struct_vwap_state* state);

int cxta_struct_vwap_step(const cxta_series_bar_view* view,
                          size_t index,
                          const cxta_struct_vwap_args* args,
                          cxta_struct_vwap_state* state,
                          cxta_struct_vwap_result* out);

#ifdef __cplusplus
}
#endif
