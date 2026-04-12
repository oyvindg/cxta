/**
 * @file hma.c
 * @brief Hull Moving Average helpers.
 */

#include <cxta/indicators/macros.h>
#include <cxta/indicators/hma.h>
#include <cxta/ts/smoothing.h>
#include <math.h>

/* WMA over bars[start..end] (inclusive) on close prices. */
static double wma_range(const cxta_series_bar* bars, size_t start, size_t end) {
    double weighted = 0.0, weights = 0.0;
    size_t rank = 1;
    for (size_t i = start; i <= end; ++i, ++rank) {
        weighted += (double)rank * bars[i].close;
        weights += (double)rank;
    }
    return (weights > 0.0) ? (weighted / weights) : 0.0;
}

/* WMA over synthetic series diff[0..len-1]. */
static double wma_array(const double* arr, size_t len) {
    double weighted = 0.0, weights = 0.0;
    for (size_t i = 0; i < len; ++i) {
        const double w = (double)(i + 1);
        weighted += w * arr[i];
        weights += w;
    }
    return (weights > 0.0) ? (weighted / weights) : 0.0;
}

double cxta_hma(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t n = (size_t)cxta_ts_clamp_period(period < 2 ? 2 : period);
    const size_t half_n = n / 2;
    const size_t sqrt_n = (size_t)(sqrt((double)n) + 0.5);
    if (sqrt_n == 0) return 0.0;

    /* Need at least sqrt_n bars for the final WMA. For each of those we need
       WMA(n) and WMA(half_n) computed at each point. */
    const size_t need = (n > sqrt_n) ? n : sqrt_n;
    if (idx + 1 < need) {
        /* Fall back: use what we have */
    }

    /* Build the diff series: for each bar k in [idx-sqrt_n+1 .. idx],
       diff[k] = 2*WMA(half_n) - WMA(n) */
    size_t diff_len = (idx + 1 < sqrt_n) ? (idx + 1) : sqrt_n;
    double diff[1024]; /* sqrt_n won't exceed 1024 for any realistic period */
    if (diff_len > 1024) diff_len = 1024;

    for (size_t d = 0; d < diff_len; ++d) {
        const size_t end_k = idx - (diff_len - 1 - d);
        /* WMA(n) at end_k */
        const size_t wn_window = (n < (end_k + 1)) ? n : (end_k + 1);
        const size_t wn_start = end_k + 1 - wn_window;
        const double wn = wma_range(view->bars, wn_start, end_k);
        /* WMA(half_n) at end_k */
        const size_t wh_window = (half_n < (end_k + 1)) ? half_n : (end_k + 1);
        const size_t wh_start = end_k + 1 - wh_window;
        const double wh = wma_range(view->bars, wh_start, end_k);
        diff[d] = 2.0 * wh - wn;
    }
    return wma_array(diff, diff_len);
}

CXTA_WRAP_BAR_SCALAR_1I(cxta_hma_desc_eval_scalar, cxta_hma, 20)

const cxta_indicator_descriptor cxta_hma_descriptor = {
    "hma",
    1,
    1,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    0u,
    NULL,
    0u,
    cxta_hma_desc_eval_scalar,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_hma_params,
    CXTA_ARRAY_COUNT(cxta_hma_params),
};
