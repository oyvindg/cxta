/**
 * @file kama.c
 * @brief KAMA helpers.
 */

#include <cxta/indicators/kama.h>
#include <cxta/ts/smoothing.h>
#include <math.h>

double cxta_kama(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    {
        const int p = cxta_ts_clamp_period(period);
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const double fast_sc = 2.0 / 3.0;
        const double slow_sc = 2.0 / 31.0;
        double kama = view->bars[0].close;

        for (size_t i = 1; i <= idx; ++i) {
            const size_t lookback = (i > (size_t)p) ? (i - (size_t)p) : 0u;
            double change = view->bars[i].close - view->bars[lookback].close;
            if (change < 0.0) change = -change;

            {
                double volatility = 0.0;
                for (size_t j = lookback + 1; j <= i; ++j) {
                    double delta = view->bars[j].close - view->bars[j - 1].close;
                    if (delta < 0.0) delta = -delta;
                    volatility += delta;
                }

                {
                    const double er = (volatility <= 1e-12) ? 0.0 : (change / volatility);
                    const double sc = pow(er * (fast_sc - slow_sc) + slow_sc, 2.0);
                    kama = kama + sc * (view->bars[i].close - kama);
                }
            }
        }
        return kama;
    }
}
