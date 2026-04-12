/**
 * @file vortex.c
 * @brief Vortex helpers.
 */

#include <cxta/indicators/vortex.h>
#include <cxta/ts/range.h>
#include <cxta/ts/smoothing.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

static const cxta_field_descriptor cxta_vortex_fields[] = {
    {"plusVI", offsetof(cxta_vortex_output, vi_plus), true},
    {"minusVI", offsetof(cxta_vortex_output, vi_minus), true},
};

static int cxta_vortex_descriptor_period_arg(const double* args,
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

static void cxta_vortex_descriptor_eval(const cxta_series_bar_view* view,
                                        const double* args,
                                        size_t nargs,
                                        void* out) {
    cxta_vortex_output value;
    memset(&value, 0, sizeof(value));
    value = cxta_vortex(view, cxta_vortex_descriptor_period_arg(args, nargs, 0u, 14));
    if (out) memcpy(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_vortex_descriptor = {
    "vortex",
    1,
    1,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_vortex_output),
    0u,
    cxta_vortex_fields,
    CXTA_ARRAY_COUNT(cxta_vortex_fields),
    NULL,
    cxta_vortex_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_vortex_params,
    CXTA_ARRAY_COUNT(cxta_vortex_params),
};

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
