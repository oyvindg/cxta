/**
 * @file stochrsi.c
 * @brief Stoch RSI helpers.
 */

#include <cxta/indicators/stochrsi.h>
#include <cxta/indicators/rsi.h>
#include <cxta/ts/smoothing.h>

static double cxta_stochrsi_rsi_at(const cxta_series_bar_view* view, size_t idx, int period) {
    const cxta_series_bar_view sub = {view->bars, idx + 1, idx};
    return cxta_rsi(&sub, period);
}

cxta_stochrsi_output cxta_stochrsi(const cxta_series_bar_view* view,
                                   int rsi_period,
                                   int stoch_period,
                                   int smooth_k,
                                   int smooth_d) {
    cxta_stochrsi_output out = {0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const size_t stoch_p = (size_t)cxta_ts_clamp_period(stoch_period);
        const size_t sk = (size_t)cxta_ts_clamp_period(smooth_k);
        const size_t sd = (size_t)cxta_ts_clamp_period(smooth_d);
        const size_t d_window = (sd < (idx + 1)) ? sd : (idx + 1);
        const size_t d_start = idx + 1 - d_window;
        double d_sum = 0.0;

        for (size_t t = d_start; t <= idx; ++t) {
            const size_t k_window = (sk < (t + 1)) ? sk : (t + 1);
            const size_t k_start = t + 1 - k_window;
            double k_sum = 0.0;

            for (size_t j = k_start; j <= t; ++j) {
                const size_t r_window = (stoch_p < (j + 1)) ? stoch_p : (j + 1);
                const size_t r_start = j + 1 - r_window;
                double rsi_min = cxta_stochrsi_rsi_at(view, r_start, rsi_period);
                double rsi_max = rsi_min;
                const double rsi_now = cxta_stochrsi_rsi_at(view, j, rsi_period);

                for (size_t i = r_start + 1; i <= j; ++i) {
                    const double r = cxta_stochrsi_rsi_at(view, i, rsi_period);
                    if (r < rsi_min) rsi_min = r;
                    if (r > rsi_max) rsi_max = r;
                }

                {
                    const double denom = rsi_max - rsi_min;
                    const double stoch = (denom <= 1e-12)
                        ? 50.0
                        : (100.0 * (rsi_now - rsi_min) / denom);
                    k_sum += stoch;
                }
            }

            {
                const double k_value = k_sum / (double)k_window;
                if (t == idx) out.k = k_value;
                d_sum += k_value;
            }
        }
        out.d = d_sum / (double)d_window;
    }
    return out;
}
