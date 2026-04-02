/**
 * @file schaff.c
 * @brief Schaff Trend Cycle helpers.
 */

#include <cxta/indicators/schaff.h>
#include <cxta/ts/smoothing.h>
#include <stdlib.h>

static double cxta_schaff_stoch_of(const double* values, size_t i, int cycle) {
    const size_t win = ((size_t)cxta_ts_clamp_period(cycle) < (i + 1))
        ? (size_t)cxta_ts_clamp_period(cycle)
        : (i + 1);
    const size_t start = i + 1 - win;
    double lo = values[start];
    double hi = values[start];

    for (size_t k = start + 1; k <= i; ++k) {
        if (values[k] < lo) lo = values[k];
        if (values[k] > hi) hi = values[k];
    }

    {
        const double d = hi - lo;
        if (d <= 1e-12) return 50.0;
        return 100.0 * (values[i] - lo) / d;
    }
}

static void cxta_schaff_apply_ema(const double* raw, size_t n, int period, double* out) {
    if (!raw || !out || n == 0) return;
    out[0] = raw[0];
    for (size_t i = 1; i < n; ++i) {
        out[i] = cxta_ts_ema_step(out[i - 1], raw[i], period);
    }
}

double cxta_schaff(const cxta_series_bar_view* view, int fast, int slow, int cycle) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;
    if (fast > slow) {
        const int tmp = fast;
        fast = slow;
        slow = tmp;
    }

    {
        const size_t n = cxta_series_clamp_index(view->size, view->index) + 1;
        double* closes = (double*)calloc(n, sizeof(double));
        double* ema_fast = (double*)calloc(n, sizeof(double));
        double* ema_slow = (double*)calloc(n, sizeof(double));
        double* macd = (double*)calloc(n, sizeof(double));
        double* fk_raw = (double*)calloc(n, sizeof(double));
        double* fk = (double*)calloc(n, sizeof(double));
        double* pf_raw = (double*)calloc(n, sizeof(double));
        double* pf = (double*)calloc(n, sizeof(double));
        double out = 0.0;

        if (!closes || !ema_fast || !ema_slow || !macd || !fk_raw || !fk || !pf_raw || !pf) {
            free(closes);
            free(ema_fast);
            free(ema_slow);
            free(macd);
            free(fk_raw);
            free(fk);
            free(pf_raw);
            free(pf);
            return 0.0;
        }

        for (size_t i = 0; i < n; ++i) closes[i] = view->bars[i].close;
        cxta_schaff_apply_ema(closes, n, fast, ema_fast);
        cxta_schaff_apply_ema(closes, n, slow, ema_slow);
        for (size_t i = 0; i < n; ++i) macd[i] = ema_fast[i] - ema_slow[i];
        for (size_t i = 0; i < n; ++i) fk_raw[i] = cxta_schaff_stoch_of(macd, i, cycle);
        cxta_schaff_apply_ema(fk_raw, n, 3, fk);
        for (size_t i = 0; i < n; ++i) pf_raw[i] = cxta_schaff_stoch_of(fk, i, cycle);
        cxta_schaff_apply_ema(pf_raw, n, 3, pf);
        out = pf[n - 1];

        free(closes);
        free(ema_fast);
        free(ema_slow);
        free(macd);
        free(fk_raw);
        free(fk);
        free(pf_raw);
        free(pf);
        return out;
    }
}
