/**
 * @file mass_index.c
 * @brief Mass Index helpers.
 */

#include <cxta/indicators/mass_index.h>
#include <cxta/ts/smoothing.h>
#include <math.h>
#include <stdlib.h>

double cxta_mass_index(const cxta_series_bar_view* view, int sum_period, int ema_period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const size_t sum_p = (size_t)cxta_ts_clamp_period(sum_period);
        const int ema_p = cxta_ts_clamp_period(ema_period);
        const double alpha = cxta_ts_ema_alpha(ema_p);
        const size_t window = (sum_p < (idx + 1)) ? sum_p : (idx + 1);
        double ema1 = view->bars[0].high - view->bars[0].low;
        double ema2 = ema1;
        double sum = 0.0;
        double* ring = (double*)calloc(window, sizeof(double));
        if (!ring) return 0.0;

        for (size_t i = 0; i <= idx; ++i) {
            double ratio = 1.0;
            if (i > 0) {
                const double range = view->bars[i].high - view->bars[i].low;
                ema1 = alpha * range + (1.0 - alpha) * ema1;
                ema2 = alpha * ema1 + (1.0 - alpha) * ema2;
            }
            if (fabs(ema2) > 1e-12) ratio = ema1 / ema2;

            if (i >= window) sum -= ring[i % window];
            ring[i % window] = ratio;
            sum += ratio;
        }

        free(ring);
        return sum;
    }
}
