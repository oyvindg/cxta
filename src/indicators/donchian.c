/**
 * @file donchian.c
 * @brief Donchian channel helpers.
 */

#include <cxta/indicators/channel.h>
#include <cxta/indicators/donchian.h>
#include <cxta/ts/smoothing.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

const cxta_field_descriptor cxta_channel_output_fields[] = {
    {"upper", offsetof(cxta_channel_output, upper), true},
    {"lower", offsetof(cxta_channel_output, lower), true},
    {"middle", offsetof(cxta_channel_output, middle), true},
    {"width", offsetof(cxta_channel_output, width), true},
};

static int cxta_donchian_descriptor_period_arg(const double* args,
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

static void cxta_donchian_descriptor_eval(const cxta_series_bar_view* view,
                                          const double* args,
                                          size_t nargs,
                                          void* out) {
    cxta_channel_output value;
    memset(&value, 0, sizeof(value));
    value = cxta_donchian(view, cxta_donchian_descriptor_period_arg(args, nargs, 0u, 20));
    if (out) memcpy(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_donchian_descriptor = {
    "donchian",
    1,
    1,
    -1,
    -1,
    2,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_channel_output),
    0u,
    cxta_channel_output_fields,
    CXTA_CHANNEL_OUTPUT_FIELD_COUNT,
    NULL,
    cxta_donchian_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_donchian_params,
    CXTA_ARRAY_COUNT(cxta_donchian_params),
};

cxta_channel_output cxta_donchian(const cxta_series_bar_view* view, int period) {
    cxta_channel_output out = {0.0, 0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t window = (p < (idx + 1)) ? p : (idx + 1);
    const size_t start = idx + 1 - window;

    out.upper = view->bars[start].high;
    out.lower = view->bars[start].low;
    for (size_t i = start + 1; i <= idx; ++i) {
        if (view->bars[i].high > out.upper) out.upper = view->bars[i].high;
        if (view->bars[i].low < out.lower) out.lower = view->bars[i].low;
    }
    out.middle = (out.upper + out.lower) * 0.5;
    out.width = out.upper - out.lower;
    return out;
}
