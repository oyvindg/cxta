/**
 * @file vwap.c
 * @brief VWAP helpers.
 */

#include <cxta/indicators/vwap.h>
#include <cxta/ts/smoothing.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>

static const cxta_field_descriptor cxta_vwap_fields[] = {
    {"value", CXTA_FIELD_OFFSET_SCALAR, true},
};

static int cxta_vwap_descriptor_int_arg(const double* args,
                                        size_t nargs,
                                        size_t index,
                                        int fallback) {
    double raw;

    if (!args || index >= nargs) return fallback;
    raw = args[index];
    if (!isfinite(raw)) return fallback;
    if (raw >= (double)INT_MAX) return INT_MAX;
    if (raw <= (double)INT_MIN) return INT_MIN;
    return (int)llround(raw);
}

static double cxta_vwap_descriptor_eval(const cxta_series_bar_view* view,
                                        const double* args,
                                        size_t nargs) {
    int period = cxta_vwap_descriptor_int_arg(args, nargs, 0u, 0);
    if (period > 0) period = cxta_ts_clamp_period(period);
    return cxta_vwap(view, period);
}

const cxta_indicator_descriptor cxta_vwap_descriptor = {
    "vwap",
    0,
    1,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    sizeof(cxta_vwap_state),
    cxta_vwap_fields,
    CXTA_ARRAY_COUNT(cxta_vwap_fields),
    cxta_vwap_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_vwap_params,
    CXTA_ARRAY_COUNT(cxta_vwap_params),
};

double cxta_vwap_step(double typical_price, double volume, cxta_vwap_state* st) {
    if (!st) return 0.0;
    st->pv_sum += typical_price * volume;
    st->v_sum += volume;
    return (st->v_sum <= 1e-12) ? 0.0 : (st->pv_sum / st->v_sum);
}

double cxta_vwap(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const size_t start = (period <= 0)
            ? 0u
            : (idx + 1 > (size_t)cxta_ts_clamp_period(period)
                ? idx + 1 - (size_t)cxta_ts_clamp_period(period)
                : 0u);
        double pv_sum = 0.0;
        double v_sum = 0.0;

        for (size_t i = start; i <= idx; ++i) {
            const double tp = cxta_series_typical_price(&view->bars[i]);
            pv_sum += tp * view->bars[i].volume;
            v_sum += view->bars[i].volume;
        }
        return (v_sum <= 1e-12) ? 0.0 : (pv_sum / v_sum);
    }
}
