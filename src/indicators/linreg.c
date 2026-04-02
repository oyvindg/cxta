/**
 * @file linreg.c
 * @brief Linear regression slope and angle helpers.
 */

#include <cxta/indicators/linreg.h>
#include <cxta/ts/smoothing.h>
#include <math.h>

/* M_PI may not be defined under strict C11 */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double cxta_linreg_slope(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t window = (p < (idx + 1)) ? p : (idx + 1);
    if (window < 2) return 0.0;
    const size_t start = idx + 1 - window;
    const double n = (double)window;

    /* Compute x as 0..n-1, y = close */
    double sx = 0.0, sy = 0.0, sxx = 0.0, sxy = 0.0;
    for (size_t i = start; i <= idx; ++i) {
        const double x = (double)(i - start);
        const double y = view->bars[i].close;
        sx += x;
        sy += y;
        sxx += x * x;
        sxy += x * y;
    }
    const double denom = n * sxx - sx * sx;
    if (denom == 0.0) return 0.0;
    return (n * sxy - sx * sy) / denom;
}

double cxta_linreg_angle(const cxta_series_bar_view* view, int period) {
    return atan(cxta_linreg_slope(view, period)) * (180.0 / M_PI);
}
