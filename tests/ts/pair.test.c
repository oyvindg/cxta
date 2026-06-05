#include <cxta/cxta.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>

static double pair_test_return(const cxta_series_bar* bars, size_t curr_index) {
    const double prev = bars[curr_index - 1u].close;
    if (curr_index == 0u) return 0.0;
    if (fabs(prev) <= 1e-12) return 0.0;
    return (bars[curr_index].close - prev) / prev;
}

void cxta_test_pair(void) {
    const cxta_series_bar bars_a[] = {
        {0u, 0.0, 0.0, 0.0, 100.0, 0.0},
        {0u, 0.0, 0.0, 0.0, 102.0, 0.0},
        {0u, 0.0, 0.0, 0.0, 101.0, 0.0},
        {0u, 0.0, 0.0, 0.0, 105.0, 0.0},
    };
    const cxta_series_bar bars_b[] = {
        {0u, 0.0, 0.0, 0.0, 50.0, 0.0},
        {0u, 0.0, 0.0, 0.0, 51.0, 0.0},
        {0u, 0.0, 0.0, 0.0, 49.0, 0.0},
        {0u, 0.0, 0.0, 0.0, 52.0, 0.0},
    };
    const cxta_series_bar_view view_a = cxta_series_bar_view_make(bars_a, 4u, 3u);
    const cxta_series_bar_view view_b = cxta_series_bar_view_make(bars_b, 4u, 3u);
    const double returns_a[] = {
        pair_test_return(bars_a, 1u),
        pair_test_return(bars_a, 2u),
        pair_test_return(bars_a, 3u),
    };
    const double returns_b[] = {
        pair_test_return(bars_b, 1u),
        pair_test_return(bars_b, 2u),
        pair_test_return(bars_b, 3u),
    };
    cxta_ts_pair_moments moments;
    double sum_a = 0.0;
    double sum_b = 0.0;
    double sum_aa = 0.0;
    double sum_bb = 0.0;
    double sum_ab = 0.0;
    double sum_diff = 0.0;
    double sum_diff_sq = 0.0;
    double expected_cov;
    double expected_beta;
    double expected_corr;
    double expected_alpha;
    double expected_hv_ratio;
    double expected_tracking_error;
    double expected_relative_strength;
    double expected_zscore;
    size_t i;

    for (i = 0u; i < 3u; ++i) {
        const double diff = returns_a[i] - returns_b[i];
        sum_a += returns_a[i];
        sum_b += returns_b[i];
        sum_aa += returns_a[i] * returns_a[i];
        sum_bb += returns_b[i] * returns_b[i];
        sum_ab += returns_a[i] * returns_b[i];
        sum_diff += diff;
        sum_diff_sq += diff * diff;
    }

    moments = cxta_ts_pair_return_moments(&view_a, &view_b, 3u);
    assert(moments.n == 3u);

    expected_cov = (sum_ab / 3.0) - ((sum_a / 3.0) * (sum_b / 3.0));
    expected_beta = expected_cov / ((sum_bb / 3.0) - pow(sum_b / 3.0, 2.0));
    expected_corr =
        expected_cov /
        sqrt(((sum_aa / 3.0) - pow(sum_a / 3.0, 2.0)) *
             ((sum_bb / 3.0) - pow(sum_b / 3.0, 2.0)));
    expected_alpha = (sum_a / 3.0) - (expected_beta * (sum_b / 3.0));
    expected_hv_ratio =
        sqrt((sum_aa / 3.0) - pow(sum_a / 3.0, 2.0)) /
        sqrt((sum_bb / 3.0) - pow(sum_b / 3.0, 2.0));
    expected_tracking_error =
        sqrt((sum_diff_sq / 3.0) - pow(sum_diff / 3.0, 2.0));

    assert(fabs(cxta_ts_spread_pair(&view_a, &view_b) - 53.0) < 1e-12);
    assert(fabs(cxta_ts_pair_spread_pair(&view_a, &view_b) - log(105.0 / 52.0)) < 1e-12);
    assert(fabs(cxta_ts_covariance_pair(&view_a, &view_b, 3u) - expected_cov) < 1e-12);
    assert(fabs(cxta_ts_rolling_beta_pair(&view_a, &view_b, 3u) - expected_beta) < 1e-12);
    assert(fabs(cxta_ts_rolling_corr_pair(&view_a, &view_b, 3u) - expected_corr) < 1e-12);
    assert(fabs(cxta_ts_rolling_alpha_pair(&view_a, &view_b, 3u) - expected_alpha) < 1e-12);
    assert(fabs(cxta_ts_hv_ratio_pair(&view_a, &view_b, 3u) - expected_hv_ratio) < 1e-12);
    assert(fabs(cxta_ts_tracking_error_pair(&view_a, &view_b, 3u) - expected_tracking_error) < 1e-12);

    expected_relative_strength =
        ((105.0 - 100.0) / 100.0) /
        ((52.0 - 50.0) / 50.0);
    assert(fabs(cxta_ts_relative_strength_pair(&view_a, &view_b, 3u) - expected_relative_strength) < 1e-12);

    {
        const double spreads[] = {50.0, 51.0, 52.0, 53.0};
        const double mean = (50.0 + 51.0 + 52.0 + 53.0) / 4.0;
        const double mean_sq =
            ((50.0 * 50.0) + (51.0 * 51.0) + (52.0 * 52.0) + (53.0 * 53.0)) / 4.0;
        expected_zscore = (spreads[3] - mean) / sqrt(mean_sq - (mean * mean));
    }
    assert(fabs(cxta_ts_zscore_pair(&view_a, &view_b, 4u) - expected_zscore) < 1e-12);

    printf("  ✓ test_pair\n");
}
