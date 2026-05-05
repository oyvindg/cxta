/**
 * @file choppiness.c
 * @brief Choppiness Index helpers.
 */

#include <cxta/indicators/macros.h>
#include <cxta/indicators/choppiness.h>
#include <cxta/ts/range.h>

static const cxta_scalar_plot_descriptor cxta_choppiness_scalar_plot =
    CXTA_SCALAR_PLOT("Choppiness", "regime", "#f59e0b", "line", "regime",
                     "Choppiness Index trend/range regime oscillator.",
                     "High values indicate range-bound conditions; low values indicate directional trend.");

static const cxta_indicator_plot_descriptor cxta_choppiness_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("choppiness_index", cxta_choppiness_scalar_plot);
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

CXTA_WRAP_BAR_SCALAR_1I(cxta_choppiness_index_desc_eval, cxta_choppiness, 14)

const cxta_indicator_descriptor cxta_choppiness_index_descriptor = {
    "choppiness_index",
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
    cxta_choppiness_index_desc_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_choppiness_index_params,
    CXTA_ARRAY_COUNT(cxta_choppiness_index_params),
    "regime",
    &cxta_choppiness_plot_descriptor,
};
