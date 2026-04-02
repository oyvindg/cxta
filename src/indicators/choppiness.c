/**
 * @file choppiness.c
 * @brief Choppiness Index helpers.
 */

#include <cxta/indicators/choppiness.h>
#include <cxta/ts/range.h>
#include <cxta/ts/smoothing.h>
#include <math.h>

double cxta_choppiness(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view) || view->size < 2) return 0.0;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        if (idx == 0) return 0.0;

        {
            const size_t p = (size_t)cxta_ts_clamp_period(period);
            const size_t window = (p < (idx + 1)) ? p : (idx + 1);
            const size_t start = idx + 1 - window;
            double hh = view->bars[start].high;
            double ll = view->bars[start].low;
            double tr_sum = 0.0;

            for (size_t i = start; i <= idx; ++i) {
                if (view->bars[i].high > hh) hh = view->bars[i].high;
                if (view->bars[i].low < ll) ll = view->bars[i].low;
                tr_sum += (i == 0)
                    ? (view->bars[i].high - view->bars[i].low)
                    : cxta_ts_true_range(view->bars[i].high, view->bars[i].low, view->bars[i - 1].close);
            }

            {
                const double range = hh - ll;
                if (range <= 0.0 || tr_sum <= 0.0 || window <= 1) return 0.0;
                return 100.0 * (log10(tr_sum / range) / log10((double)window));
            }
        }
    }
}
