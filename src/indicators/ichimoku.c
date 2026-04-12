/**
 * @file ichimoku.c
 * @brief Ichimoku Cloud helpers.
 */

#include <cxta/indicators/ichimoku.h>
#include <cxta/ts/smoothing.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

static const cxta_field_descriptor cxta_ichimoku_fields[] = {
    {"tenkan", offsetof(cxta_ichimoku_output, tenkan), true},
    {"kijun", offsetof(cxta_ichimoku_output, kijun), true},
    {"senkouA", offsetof(cxta_ichimoku_output, senkou_a), true},
    {"senkouB", offsetof(cxta_ichimoku_output, senkou_b), true},
    {"chikou", offsetof(cxta_ichimoku_output, chikou), true},
};

static int cxta_ichimoku_descriptor_int_arg(const double* args,
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

static int cxta_ichimoku_descriptor_period_arg(const double* args,
                                             size_t nargs,
                                             size_t index,
                                             int fallback) {
    return cxta_ts_clamp_period(cxta_ichimoku_descriptor_int_arg(args, nargs, index, fallback));
}

static void cxta_ichimoku_descriptor_eval(const cxta_series_bar_view* view,
                                        const double* args,
                                        size_t nargs,
                                        void* out) {
    int tenkan = cxta_ichimoku_descriptor_period_arg(args, nargs, 0u, 9);
    int kijun = cxta_ichimoku_descriptor_period_arg(args, nargs, 1u, 26);
    int senkou_b = cxta_ichimoku_descriptor_period_arg(args, nargs, 2u, 52);
    int swap_tmp;
    cxta_ichimoku_output value;

    if (tenkan > kijun) {
        swap_tmp = tenkan;
        tenkan = kijun;
        kijun = swap_tmp;
    }
    if (senkou_b < kijun + 1) senkou_b = kijun + 1;
    value = cxta_ichimoku(view, tenkan, kijun, senkou_b);
    if (out) memcpy(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_ichimoku_descriptor = {
    "ichimoku",
    3,
    3,
    -1,
    -1,
    1,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_ichimoku_output),
    0u,
    cxta_ichimoku_fields,
    CXTA_ARRAY_COUNT(cxta_ichimoku_fields),
    NULL,
    cxta_ichimoku_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_ichimoku_params,
    CXTA_ARRAY_COUNT(cxta_ichimoku_params),
};

static double cxta_ichimoku_midpoint_window(const cxta_series_bar_view* view,
                                            size_t idx,
                                            int period) {
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t window = (p < (idx + 1)) ? p : (idx + 1);
    const size_t start = idx + 1 - window;
    double highest = view->bars[start].high;
    double lowest = view->bars[start].low;

    for (size_t i = start + 1; i <= idx; ++i) {
        if (view->bars[i].high > highest) highest = view->bars[i].high;
        if (view->bars[i].low < lowest) lowest = view->bars[i].low;
    }
    return (highest + lowest) * 0.5;
}

cxta_ichimoku_output cxta_ichimoku(const cxta_series_bar_view* view,
                                   int tenkan_period,
                                   int kijun_period,
                                   int senkou_b_period) {
    cxta_ichimoku_output out = {0.0, 0.0, 0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    if (tenkan_period > kijun_period) {
        const int tmp = tenkan_period;
        tenkan_period = kijun_period;
        kijun_period = tmp;
    }
    if (senkou_b_period <= kijun_period) senkou_b_period = kijun_period + 1;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        out.tenkan = cxta_ichimoku_midpoint_window(view, idx, tenkan_period);
        out.kijun = cxta_ichimoku_midpoint_window(view, idx, kijun_period);
        out.senkou_a = (out.tenkan + out.kijun) * 0.5;
        out.senkou_b = cxta_ichimoku_midpoint_window(view, idx, senkou_b_period);
        out.chikou = view->bars[idx].close;
    }
    return out;
}
