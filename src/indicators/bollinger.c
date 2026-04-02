/**
 * @file bollinger.c
 * @brief Bollinger band helpers.
 */

#include <cxta/indicators/bollinger.h>
#include <cxta/math/math.h>
#include <cxta/ts/smoothing.h>
#include <math.h>

cxta_bollinger_output cxta_bollinger(const cxta_series_bar_view* view,
                                     int period,
                                     double stddev_mult) {
    cxta_bollinger_output out = {0.0, 0.0, 0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t window = (p < (idx + 1)) ? p : (idx + 1);
    const size_t start = idx + 1 - window;

    double sum = 0.0;
    double sum_sq = 0.0;
    for (size_t i = start; i <= idx; ++i) {
        const double close = view->bars[i].close;
        sum += close;
        sum_sq += close * close;
    }

    const double mean = sum / (double)window;
    const double variance = cxta_math_max2(0.0, (sum_sq / (double)window) - (mean * mean));
    const double sd = sqrt(variance);

    out.middle = mean;
    out.upper = mean + (stddev_mult * sd);
    out.lower = mean - (stddev_mult * sd);
    out.bandwidth = (fabs(out.middle) > 1e-12) ? ((out.upper - out.lower) / out.middle) : 0.0;
    out.percent_b = (fabs(out.upper - out.lower) > 1e-12)
        ? ((view->bars[idx].close - out.lower) / (out.upper - out.lower))
        : 0.5;
    return out;
}
