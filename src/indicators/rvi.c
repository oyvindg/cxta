/**
 * @file rvi.c
 * @brief Relative Vigor Index helpers.
 */

#include <cxta/indicators/rvi.h>
#include <cxta/ts/smoothing.h>

double cxta_rvi(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        if (idx < 3) return 0.0;

        {
            const size_t p = (size_t)cxta_ts_clamp_period(period);
            const size_t available = idx - 2;
            const size_t window = (p < available) ? p : available;
            const size_t start = idx + 1 - window;
            double num = 0.0;
            double den = 0.0;

            for (size_t i = start; i <= idx; ++i) {
                num += view->bars[i].close - view->bars[i].open
                    + 2.0 * (view->bars[i - 1].close - view->bars[i - 1].open)
                    + 2.0 * (view->bars[i - 2].close - view->bars[i - 2].open)
                    + (view->bars[i - 3].close - view->bars[i - 3].open);
                den += view->bars[i].high - view->bars[i].low
                    + 2.0 * (view->bars[i - 1].high - view->bars[i - 1].low)
                    + 2.0 * (view->bars[i - 2].high - view->bars[i - 2].low)
                    + (view->bars[i - 3].high - view->bars[i - 3].low);
            }

            if (den <= 1e-12) return 0.0;
            return num / den;
        }
    }
}
