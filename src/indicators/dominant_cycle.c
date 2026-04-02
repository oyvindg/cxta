/**
 * @file dominant_cycle.c
 * @brief Dominant cycle period helpers.
 */

#include <cxta/indicators/dominant_cycle.h>
#include <cxta/ts/smoothing.h>
#include <math.h>

static double cxta_dominant_cycle_abs(double x) {
    return (x < 0.0) ? -x : x;
}

static double cxta_dominant_cycle_sqrt_max0(double x) {
    return sqrt((x < 0.0) ? 0.0 : x);
}

double cxta_dominant_cycle(const cxta_series_bar_view* view,
                           int min_period,
                           int max_period) {
    if (!view || !cxta_series_bar_view_valid(view) || view->size < 4) return 0.0;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        int min_p = cxta_ts_clamp_period(min_period);
        int max_p = cxta_ts_clamp_period(max_period);
        if (min_p < 5) min_p = 5;
        if (max_p < min_p + 2) max_p = min_p + 2;

        if (idx < (size_t)(max_p + 2)) return (double)min_p;

        {
            const size_t window = (size_t)(max_p * 2);
            const size_t start = (idx > window) ? (idx - window) : 1u;
            int best_period = min_p;
            double best_score = -1.0;

            for (int lag = min_p; lag <= max_p; ++lag) {
                double sx = 0.0;
                double sy = 0.0;
                double sxx = 0.0;
                double syy = 0.0;
                double sxy = 0.0;
                int n = 0;

                for (size_t i = start + (size_t)lag; i <= idx; ++i) {
                    const double x = view->bars[i].close - view->bars[i - 1].close;
                    const double y =
                        view->bars[i - (size_t)lag].close - view->bars[i - (size_t)lag - 1].close;
                    sx += x;
                    sy += y;
                    sxx += x * x;
                    syy += y * y;
                    sxy += x * y;
                    ++n;
                }

                if (n < 4) continue;

                {
                    const double num = (double)n * sxy - sx * sy;
                    const double den_l = (double)n * sxx - sx * sx;
                    const double den_r = (double)n * syy - sy * sy;
                    const double den = cxta_dominant_cycle_sqrt_max0(den_l) *
                        cxta_dominant_cycle_sqrt_max0(den_r);
                    const double corr = (den <= 1e-12) ? 0.0 : cxta_dominant_cycle_abs(num / den);
                    if (corr > best_score) {
                        best_score = corr;
                        best_period = lag;
                    }
                }
            }
            return (double)best_period;
        }
    }
}
