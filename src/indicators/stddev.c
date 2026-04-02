/**
 * @file stddev.c
 * @brief Rolling standard deviation helpers.
 */

#include <cxta/indicators/stddev.h>
#include <cxta/ts/smoothing.h>
#include <cxta/ts/rolling.h>
#include <math.h>

double cxta_stddev(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t window = (p < (idx + 1)) ? p : (idx + 1);
    if (window < 2) return 0.0;
    const size_t start = idx + 1 - window;

    cxta_ts_rolling_state st;
    cxta_ts_rolling_reset(&st);
    for (size_t i = start; i <= idx; ++i) {
        cxta_ts_rolling_add(&st, view->bars[i].close);
    }
    return sqrt(cxta_ts_rolling_var(&st));
}
