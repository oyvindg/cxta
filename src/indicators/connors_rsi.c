/**
 * @file connors_rsi.c
 * @brief Connors RSI helpers.
 */

#include <cxta/indicators/connors_rsi.h>
#include <cxta/ts/smoothing.h>
#include <stdlib.h>

static double cxta_connors_rsi_from_series(const double* values, size_t count, int period) {
    const int p = cxta_ts_clamp_period(period);
    if (!values || count <= (size_t)p) return 50.0;

    {
        double avg_gain = 0.0;
        double avg_loss = 0.0;

        for (int i = 1; i <= p; ++i) {
            const double d = values[(size_t)i] - values[(size_t)(i - 1)];
            if (d > 0.0) avg_gain += d;
            else avg_loss -= d;
        }
        avg_gain /= (double)p;
        avg_loss /= (double)p;

        for (size_t i = (size_t)p + 1; i < count; ++i) {
            const double d = values[i] - values[i - 1];
            const double gain = (d > 0.0) ? d : 0.0;
            const double loss = (d < 0.0) ? -d : 0.0;
            avg_gain = ((avg_gain * (double)(p - 1)) + gain) / (double)p;
            avg_loss = ((avg_loss * (double)(p - 1)) + loss) / (double)p;
        }

        if (avg_loss <= 1e-12) return 100.0;
        {
            const double rs = avg_gain / avg_loss;
            return 100.0 - (100.0 / (1.0 + rs));
        }
    }
}

double cxta_connors_rsi(const cxta_series_bar_view* view,
                        int rsi_period,
                        int streak_period,
                        int rank_period) {
    if (!view || !cxta_series_bar_view_valid(view) || view->size < 2) return 50.0;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        double* closes = NULL;
        double* streak = NULL;
        double close_rsi = 50.0;
        double streak_rsi = 50.0;
        double percent_rank = 50.0;

        if (idx == 0) return 50.0;

        closes = (double*)calloc(idx + 1, sizeof(double));
        streak = (double*)calloc(idx + 1, sizeof(double));
        if (!closes || !streak) {
            free(closes);
            free(streak);
            return 50.0;
        }

        for (size_t i = 0; i <= idx; ++i) closes[i] = view->bars[i].close;
        close_rsi = cxta_connors_rsi_from_series(closes, idx + 1, rsi_period);

        streak[0] = 0.0;
        for (size_t i = 1; i <= idx; ++i) {
            const double diff = view->bars[i].close - view->bars[i - 1].close;
            if (diff > 0.0) {
                streak[i] = (streak[i - 1] > 0.0) ? (streak[i - 1] + 1.0) : 1.0;
            } else if (diff < 0.0) {
                streak[i] = (streak[i - 1] < 0.0) ? (streak[i - 1] - 1.0) : -1.0;
            } else {
                streak[i] = 0.0;
            }
        }
        streak_rsi = cxta_connors_rsi_from_series(streak, idx + 1, streak_period);

        if (idx >= (size_t)(rank_period + 1)) {
            size_t less_count = 0;
            const size_t start = idx - (size_t)rank_period;
            const double curr_roc1 = (view->bars[idx].close / view->bars[idx - 1].close) - 1.0;
            for (size_t i = start; i < idx; ++i) {
                const double roc1 = (view->bars[i].close / view->bars[i - 1].close) - 1.0;
                if (roc1 < curr_roc1) ++less_count;
            }
            percent_rank = 100.0 * ((double)less_count / (double)rank_period);
        }

        free(closes);
        free(streak);
        return (close_rsi + streak_rsi + percent_rank) / 3.0;
    }
}
