/**
 * @file pair.c
 * @brief Pairwise cross-series helpers over bar views.
 */

#include <cxta/ts/pair.h>

#include <math.h>

static const double cxta_pair_epsilon = 1e-12;

static int cxta_ts_pair_valid(const cxta_series_bar_view* view) {
    return view != NULL && cxta_series_bar_view_valid(view) != 0;
}

static size_t cxta_ts_pair_index(const cxta_series_bar_view* view) {
    return cxta_series_clamp_index(view->size, view->index);
}

static double cxta_ts_pair_simple_return(const cxta_series_bar_view* view, size_t curr_index) {
    if (curr_index == 0u || curr_index >= view->size) return 0.0;
    const double prev_close = view->bars[curr_index - 1u].close;
    if (fabs(prev_close) <= cxta_pair_epsilon) return 0.0;
    return (view->bars[curr_index].close - prev_close) / prev_close;
}

cxta_ts_pair_moments cxta_ts_pair_return_moments(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b,
    size_t period) {
    cxta_ts_pair_moments out = {0};
    size_t idx_a;
    size_t idx_b;
    size_t max_window;
    size_t window;
    double sum_a = 0.0;
    double sum_b = 0.0;
    double sum_aa = 0.0;
    double sum_bb = 0.0;
    double sum_ab = 0.0;
    double sum_diff = 0.0;
    double sum_diff_sq = 0.0;
    size_t i;

    if (!cxta_ts_pair_valid(a) || !cxta_ts_pair_valid(b)) return out;

    idx_a = cxta_ts_pair_index(a);
    idx_b = cxta_ts_pair_index(b);
    max_window = idx_a < idx_b ? idx_a : idx_b;
    window = period < max_window ? period : max_window;
    if (window == 0u) return out;

    for (i = 0u; i < window; ++i) {
        const size_t offset = window - 1u - i;
        const size_t a_curr = idx_a - offset;
        const size_t b_curr = idx_b - offset;
        const double r_a = cxta_ts_pair_simple_return(a, a_curr);
        const double r_b = cxta_ts_pair_simple_return(b, b_curr);
        const double diff = r_a - r_b;
        sum_a += r_a;
        sum_b += r_b;
        sum_aa += r_a * r_a;
        sum_bb += r_b * r_b;
        sum_ab += r_a * r_b;
        sum_diff += diff;
        sum_diff_sq += diff * diff;
    }

    {
        const double n = (double)window;
        out.n = window;
        out.mean_a = sum_a / n;
        out.mean_b = sum_b / n;
        out.cov = (sum_ab / n) - (out.mean_a * out.mean_b);
        out.var_a = (sum_aa / n) - (out.mean_a * out.mean_a);
        out.var_b = (sum_bb / n) - (out.mean_b * out.mean_b);
        out.mean_diff = sum_diff / n;
        out.var_diff = (sum_diff_sq / n) - (out.mean_diff * out.mean_diff);
    }

    if (out.var_a < 0.0 && fabs(out.var_a) <= cxta_pair_epsilon) out.var_a = 0.0;
    if (out.var_b < 0.0 && fabs(out.var_b) <= cxta_pair_epsilon) out.var_b = 0.0;
    if (out.var_diff < 0.0 && fabs(out.var_diff) <= cxta_pair_epsilon) out.var_diff = 0.0;
    return out;
}

double cxta_ts_spread_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b) {
    if (!cxta_ts_pair_valid(a) || !cxta_ts_pair_valid(b)) return 0.0;
    const size_t idx_a = cxta_ts_pair_index(a);
    const size_t idx_b = cxta_ts_pair_index(b);
    return a->bars[idx_a].close - b->bars[idx_b].close;
}

double cxta_ts_pair_spread_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b) {
    if (!cxta_ts_pair_valid(a) || !cxta_ts_pair_valid(b)) return 0.0;
    const size_t idx_a = cxta_ts_pair_index(a);
    const size_t idx_b = cxta_ts_pair_index(b);
    const double close_a = a->bars[idx_a].close;
    const double close_b = b->bars[idx_b].close;
    if (close_a <= cxta_pair_epsilon || close_b <= cxta_pair_epsilon) return 0.0;
    return log(close_a / close_b);
}

double cxta_ts_covariance_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b,
    size_t period) {
    const cxta_ts_pair_moments moments = cxta_ts_pair_return_moments(a, b, period);
    return moments.n == 0u ? 0.0 : moments.cov;
}

double cxta_ts_rolling_corr_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b,
    size_t period) {
    const cxta_ts_pair_moments moments = cxta_ts_pair_return_moments(a, b, period);
    const double denom = sqrt(moments.var_a * moments.var_b);
    if (moments.n == 0u || denom <= cxta_pair_epsilon) return 0.0;
    return moments.cov / denom;
}

double cxta_ts_rolling_beta_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b,
    size_t period) {
    const cxta_ts_pair_moments moments = cxta_ts_pair_return_moments(a, b, period);
    if (moments.n == 0u || moments.var_b <= cxta_pair_epsilon) return 0.0;
    return moments.cov / moments.var_b;
}

double cxta_ts_rolling_alpha_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b,
    size_t period) {
    const cxta_ts_pair_moments moments = cxta_ts_pair_return_moments(a, b, period);
    const double beta =
        moments.var_b <= cxta_pair_epsilon ? 0.0 : (moments.cov / moments.var_b);
    if (moments.n == 0u) return 0.0;
    return moments.mean_a - (beta * moments.mean_b);
}

double cxta_ts_hv_ratio_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b,
    size_t period) {
    const cxta_ts_pair_moments moments = cxta_ts_pair_return_moments(a, b, period);
    const double std_b = sqrt(fmax(0.0, moments.var_b));
    if (moments.n == 0u || std_b <= cxta_pair_epsilon) return 0.0;
    return sqrt(fmax(0.0, moments.var_a)) / std_b;
}

double cxta_ts_tracking_error_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b,
    size_t period) {
    const cxta_ts_pair_moments moments = cxta_ts_pair_return_moments(a, b, period);
    if (moments.n == 0u || moments.var_diff <= cxta_pair_epsilon) return 0.0;
    return sqrt(fmax(0.0, moments.var_diff));
}

double cxta_ts_relative_strength_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b,
    size_t period) {
    size_t idx_a;
    size_t idx_b;
    size_t max_lookback;
    size_t lookback;
    double start_a;
    double start_b;
    double perf_a;
    double perf_b;

    if (!cxta_ts_pair_valid(a) || !cxta_ts_pair_valid(b)) return 0.0;

    idx_a = cxta_ts_pair_index(a);
    idx_b = cxta_ts_pair_index(b);
    max_lookback = idx_a < idx_b ? idx_a : idx_b;
    lookback = period < max_lookback ? period : max_lookback;
    if (lookback == 0u) return 0.0;

    start_a = a->bars[idx_a - lookback].close;
    start_b = b->bars[idx_b - lookback].close;
    if (fabs(start_a) <= cxta_pair_epsilon || fabs(start_b) <= cxta_pair_epsilon) return 0.0;

    perf_a = (a->bars[idx_a].close - start_a) / start_a;
    perf_b = (b->bars[idx_b].close - start_b) / start_b;
    if (fabs(perf_b) <= cxta_pair_epsilon) return 0.0;
    return perf_a / perf_b;
}

double cxta_ts_zscore_pair(
    const cxta_series_bar_view* a,
    const cxta_series_bar_view* b,
    size_t period) {
    size_t idx_a;
    size_t idx_b;
    size_t max_points;
    size_t points;
    double sum = 0.0;
    double sum_sq = 0.0;
    size_t i;
    double mean;
    double var;
    double current_spread;

    if (!cxta_ts_pair_valid(a) || !cxta_ts_pair_valid(b)) return 0.0;

    idx_a = cxta_ts_pair_index(a);
    idx_b = cxta_ts_pair_index(b);
    max_points = (idx_a < idx_b ? idx_a : idx_b) + 1u;
    points = period < max_points ? period : max_points;
    if (points == 0u) return 0.0;

    for (i = 0u; i < points; ++i) {
        const size_t offset = points - 1u - i;
        const double spread =
            a->bars[idx_a - offset].close - b->bars[idx_b - offset].close;
        sum += spread;
        sum_sq += spread * spread;
    }

    mean = sum / (double)points;
    var = (sum_sq / (double)points) - (mean * mean);
    if (var < 0.0 && fabs(var) <= cxta_pair_epsilon) var = 0.0;
    if (var <= cxta_pair_epsilon) return 0.0;

    current_spread = a->bars[idx_a].close - b->bars[idx_b].close;
    return (current_spread - mean) / sqrt(var);
}
