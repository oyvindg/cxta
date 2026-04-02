/**
 * @file vortex.c
 * @brief Vortex helpers.
 */

#include <cxta/indicators/vortex.h>
#include <cxta/ts/range.h>
#include <cxta/ts/smoothing.h>
#include <math.h>

cxta_vortex_output cxta_vortex(const cxta_series_bar_view* view, int period) {
    cxta_vortex_output out = {0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view) || view->size < 2) return out;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        if (idx == 0) return out;

        {
            const size_t p = (size_t)cxta_ts_clamp_period(period);
            const size_t usable = (p < idx) ? p : idx;
            const size_t start = idx - usable + 1;
            double vm_plus = 0.0;
            double vm_minus = 0.0;
            double tr_sum = 0.0;

            for (size_t i = start; i <= idx; ++i) {
                vm_plus += fabs(view->bars[i].high - view->bars[i - 1].low);
                vm_minus += fabs(view->bars[i].low - view->bars[i - 1].high);
                tr_sum += cxta_ts_true_range(view->bars[i].high, view->bars[i].low, view->bars[i - 1].close);
            }

            if (tr_sum <= 0.0) return out;
            out.vi_plus = vm_plus / tr_sum;
            out.vi_minus = vm_minus / tr_sum;
            out.trend = out.vi_plus - out.vi_minus;
        }
    }
    return out;
}
