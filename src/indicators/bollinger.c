/**
 * @file bollinger.c
 * @brief Bollinger band helpers.
 */

#include <limits.h>
#include <math.h>
#include <string.h>

#include <cxta/indicators/bollinger.h>
#include <cxta/math/math.h>
#include <cxta/ts/smoothing.h>

static const cxta_field_descriptor cxta_bollinger_fields[] = {
    {"upper", offsetof(cxta_bollinger_output, upper), true},
    {"lower", offsetof(cxta_bollinger_output, lower), true},
    {"middle", offsetof(cxta_bollinger_output, middle), true},
    {"percentB", offsetof(cxta_bollinger_output, percent_b), true},
    {"width", offsetof(cxta_bollinger_output, bandwidth), true},
};

static int cxta_bollinger_descriptor_period_arg(const double* args,
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

static double cxta_bollinger_descriptor_double_arg(const double* args,
                                                   size_t nargs,
                                                   size_t index,
                                                   double fallback) {
    double raw;

    if (!args || index >= nargs) return fallback;
    raw = args[index];
    return isfinite(raw) ? raw : fallback;
}

static void cxta_bollinger_descriptor_copy_struct(void* out,
                                                  const void* value,
                                                  size_t size) {
    if (!out || !value || size == 0u) return;
    memcpy(out, value, size);
}

static void cxta_bollinger_descriptor_eval(const cxta_series_bar_view* view,
                                           const double* args,
                                           size_t nargs,
                                           void* out) {
    cxta_bollinger_output value;

    memset(&value, 0, sizeof(value));
    value = cxta_bollinger(
        view,
        cxta_bollinger_descriptor_period_arg(args, nargs, 0u, 20),
        cxta_bollinger_descriptor_double_arg(args, nargs, 1u, 2.0));
    cxta_bollinger_descriptor_copy_struct(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_bollinger_descriptor = {
    "bollinger",
    2,
    2,
    -1,
    -1,
    2,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_bollinger_output),
    0u,
    cxta_bollinger_fields,
    CXTA_ARRAY_COUNT(cxta_bollinger_fields),
    NULL,
    cxta_bollinger_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_bollinger_params,
    CXTA_ARRAY_COUNT(cxta_bollinger_params),
};

cxta_bollinger_output cxta_bollinger(const cxta_series_bar_view* view,
                                     int period,
                                     double stddev_mult) {
    cxta_bollinger_output out = {0.0, 0.0, 0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t p = (size_t)cxta_ts_clamp_period(period);
    const size_t window = (p < (idx + 1u)) ? p : (idx + 1u);
    const size_t start = idx + 1u - window;

    double sum = 0.0;
    double sum_sq = 0.0;
    for (size_t i = start; i <= idx; ++i) {
        const double close = view->bars[i].close;
        sum += close;
        sum_sq += close * close;
    }

    const double mean = sum / (double)window;
    const double variance = cxta_math_max2(0.0, (sum_sq / (double)window) - (mean * mean));
    const double sd = sqrt(variance);

    out.middle = mean;
    out.upper = mean + (stddev_mult * sd);
    out.lower = mean - (stddev_mult * sd);
    out.bandwidth = (fabs(out.middle) > 1e-12) ? ((out.upper - out.lower) / out.middle) : 0.0;
    out.percent_b = (fabs(out.upper - out.lower) > 1e-12)
        ? ((view->bars[idx].close - out.lower) / (out.upper - out.lower))
        : 0.5;
    return out;
}
