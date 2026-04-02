/**
 * @file vidya.c
 * @brief VIDYA helpers.
 */

#include <cxta/indicators/vidya.h>
#include <cxta/ts/smoothing.h>
#include <math.h>

double cxta_vidya(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    {
        const int p = cxta_ts_clamp_period(period);
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const double base_a = 2.0 / ((double)p + 1.0);
        double value = view->bars[0].close;

        for (size_t i = 1; i <= idx; ++i) {
            const size_t start = (i > (size_t)p) ? (i - (size_t)p) : 1u;
            double up = 0.0;
            double down = 0.0;

            for (size_t j = start; j <= i; ++j) {
                const double d = view->bars[j].close - view->bars[j - 1].close;
                if (d > 0.0) up += d;
                else down -= d;
            }

            {
                const double denom = up + down;
                const double cmo_abs = (denom <= 1e-12) ? 0.0 : fabs((up - down) / denom);
                const double a = base_a * cmo_abs;
                value = value + a * (view->bars[i].close - value);
            }
        }
        return value;
    }
}
