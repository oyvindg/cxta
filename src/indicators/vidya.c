/**
 * @file vidya.c
 * @brief VIDYA helpers.
 */

#include <limits.h>
#include <math.h>

#include <cxta/indicators/vidya.h>
#include <cxta/ts/smoothing.h>

static int cxta_vidya_descriptor_period_arg(const double* args,
                                            size_t nargs,
                                            size_t index,
                                            int fallback) {
    double raw;

    if (!args || index >= nargs) return fallback;
    raw = args[index];
    if (!isfinite(raw)) return fallback;
    if (raw >= (double)INT_MAX) return INT_MAX;
    if (raw <= (double)INT_MIN) return INT_MIN;
    return cxta_ts_clamp_period((int)llround(raw));
}

static double cxta_vidya_descriptor_eval(const cxta_series_bar_view* view,
                                         const double* args,
                                         size_t nargs) {
    return cxta_vidya(view, cxta_vidya_descriptor_period_arg(args, nargs, 0u, 14));
}

const cxta_indicator_descriptor cxta_vidya_descriptor = {
    "vidya",
    1,
    1,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    0u,
    NULL,
    0u,
    cxta_vidya_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_vidya_params,
    CXTA_ARRAY_COUNT(cxta_vidya_params),
};

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
