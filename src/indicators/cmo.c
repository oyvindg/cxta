/**
 * @file cmo.c
 * @brief Chande Momentum Oscillator helpers.
 */

#include <cxta/indicators/macros.h>
#include <cxta/indicators/cmo.h>
#include <cxta/ts/smoothing.h>

double cxta_cmo(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view) || view->size < 2) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    if (idx == 0) return 0.0;

    {
        const size_t p = (size_t)cxta_ts_clamp_period(period);
        const size_t window = (p < idx) ? p : idx;
        const size_t start = idx - window + 1;
        double up = 0.0;
        double down = 0.0;

        for (size_t i = start; i <= idx; ++i) {
            const double d = view->bars[i].close - view->bars[i - 1].close;
            if (d > 0.0) up += d;
            else down -= d;
        }

        {
            const double denom = up + down;
            if (denom <= 1e-12) return 0.0;
            return 100.0 * ((up - down) / denom);
        }
    }
}

CXTA_WRAP_BAR_SCALAR_1I(cxta_cmo_desc_eval, cxta_cmo, 14)

const cxta_indicator_descriptor cxta_cmo_descriptor = {
    "cmo",
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
    cxta_cmo_desc_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_cmo_params,
    CXTA_ARRAY_COUNT(cxta_cmo_params),
};
