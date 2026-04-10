/**
 * @file pair.h
 * @brief Pairwise cross-series helpers over bar views.
 */

#pragma once

#include <cxta/series/bar.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    size_t n;
    double mean_a;
    double mean_b;
    double cov;
    double var_a;
    double var_b;
    double mean_diff;
    double var_diff;
} cxta_ts_pair_moments;

cxta_ts_pair_moments cxta_ts_pair_return_moments(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b,
    size_t period);

double cxta_ts_spread_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b);

double cxta_ts_pair_spread_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b);

double cxta_ts_covariance_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b,
    size_t period);

double cxta_ts_rolling_corr_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b,
    size_t period);

double cxta_ts_rolling_beta_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b,
    size_t period);

double cxta_ts_rolling_alpha_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b,
    size_t period);

double cxta_ts_hv_ratio_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b,
    size_t period);

double cxta_ts_tracking_error_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b,
    size_t period);

double cxta_ts_relative_strength_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b,
    size_t period);

double cxta_ts_z_score_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b,
    size_t period);

#ifdef __cplusplus
}
#endif
