/**
 * @file rolling_extrema.c
 * @brief Rolling window min/max descriptor implementations.
 */

#include <cxta/indicators/rolling_extrema.h>
#include <cxta/series/bar.h>
#include <cxta/ts/smoothing.h>

#include <limits.h>
#include <math.h>
#include <stddef.h>

static int cxta_rolling_extrema_period_arg(const double* args, size_t nargs, int fallback) {
    double raw;

    if (!args || nargs == 0u) return cxta_ts_clamp_period(fallback);
    raw = args[0];
    if (!isfinite(raw)) return cxta_ts_clamp_period(fallback);
    if (raw >= (double)INT_MAX) return INT_MAX;
    if (raw <= (double)INT_MIN) return INT_MIN;
    return cxta_ts_clamp_period((int)llround(raw));
}

static double cxta_rolling_bar_window_extrema(const cxta_series_bar_view* view,
                                              const double* args,
                                              size_t nargs,
                                              size_t field_offset,
                                              int want_max) {
    size_t idx;
    size_t start;
    size_t window;
    size_t i;
    double value;
    const cxta_series_bar* bar;

    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;
    idx = cxta_series_clamp_index(view->size, view->index);
    window = (size_t)cxta_rolling_extrema_period_arg(args, nargs, 1);
    if (window > idx + 1u) window = idx + 1u;
    start = idx + 1u - window;
    bar = &view->bars[start];
    value = *(const double*)((const unsigned char*)bar + field_offset);

    for (i = start + 1u; i <= idx; ++i) {
        double sample;
        bar = &view->bars[i];
        sample = *(const double*)((const unsigned char*)bar + field_offset);
        if (want_max) {
            if (sample > value) value = sample;
        } else {
            if (sample < value) value = sample;
        }
    }

    return value;
}

static double cxta_rolling_scalar_window_extrema(const cxta_series_scalar_view* source,
                                                 const double* args,
                                                 size_t nargs,
                                                 int want_max) {
    size_t idx;
    size_t start;
    size_t window;
    size_t i;
    double value;

    if (!source || !cxta_series_scalar_view_valid(source)) return 0.0;
    idx = cxta_series_clamp_index(source->size, source->index);
    window = (size_t)cxta_rolling_extrema_period_arg(args, nargs, 1);
    if (window > idx + 1u) window = idx + 1u;
    start = idx + 1u - window;
    value = source->values[start];

    for (i = start + 1u; i <= idx; ++i) {
        if (want_max) {
            if (source->values[i] > value) value = source->values[i];
        } else {
            if (source->values[i] < value) value = source->values[i];
        }
    }

    return value;
}

static double cxta_rolling_max_eval(const cxta_series_bar_view* view,
                                    const double* args,
                                    size_t nargs) {
    return cxta_rolling_bar_window_extrema(
        view, args, nargs, offsetof(cxta_series_bar, high), 1);
}

static double cxta_rolling_min_eval(const cxta_series_bar_view* view,
                                      const double* args,
                                      size_t nargs) {
    return cxta_rolling_bar_window_extrema(
        view, args, nargs, offsetof(cxta_series_bar, low), 0);
}

static double cxta_rolling_max_close_eval(const cxta_series_bar_view* view,
                                          const double* args,
                                          size_t nargs) {
    return cxta_rolling_bar_window_extrema(
        view, args, nargs, offsetof(cxta_series_bar, close), 1);
}

static double cxta_rolling_min_close_eval(const cxta_series_bar_view* view,
                                          const double* args,
                                          size_t nargs) {
    return cxta_rolling_bar_window_extrema(
        view, args, nargs, offsetof(cxta_series_bar, close), 0);
}

static double cxta_rolling_max_scalar_src(const cxta_series_scalar_view* source,
                                          const double* args,
                                          size_t nargs) {
    return cxta_rolling_scalar_window_extrema(source, args, nargs, 1);
}

static double cxta_rolling_min_scalar_src(const cxta_series_scalar_view* source,
                                          const double* args,
                                          size_t nargs) {
    return cxta_rolling_scalar_window_extrema(source, args, nargs, 0);
}

const cxta_indicator_descriptor cxta_rolling_max_descriptor = {
    "rolling_max",
    1,
    1,
    1,
    1,
    -1,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_SCALAR_SOURCE,
    0u,
    0u,
    NULL,
    0u,
    cxta_rolling_max_eval,
    NULL,
    cxta_rolling_max_scalar_src,
    NULL,
    NULL,
    NULL,
    cxta_rolling_extrema_params,
    CXTA_ARRAY_COUNT(cxta_rolling_extrema_params),
};

const cxta_indicator_descriptor cxta_rolling_min_descriptor = {
    "rolling_min",
    1,
    1,
    1,
    1,
    -1,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_SCALAR_SOURCE,
    0u,
    0u,
    NULL,
    0u,
    cxta_rolling_min_eval,
    NULL,
    cxta_rolling_min_scalar_src,
    NULL,
    NULL,
    NULL,
    cxta_rolling_extrema_params,
    CXTA_ARRAY_COUNT(cxta_rolling_extrema_params),
};

const cxta_indicator_descriptor cxta_rolling_max_close_descriptor = {
    "rolling_max_close",
    1,
    1,
    1,
    1,
    -1,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_SCALAR_SOURCE,
    0u,
    0u,
    NULL,
    0u,
    cxta_rolling_max_close_eval,
    NULL,
    cxta_rolling_max_scalar_src,
    NULL,
    NULL,
    NULL,
    cxta_rolling_extrema_params,
    CXTA_ARRAY_COUNT(cxta_rolling_extrema_params),
};

const cxta_indicator_descriptor cxta_rolling_min_close_descriptor = {
    "rolling_min_close",
    1,
    1,
    1,
    1,
    -1,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_SCALAR_SOURCE,
    0u,
    0u,
    NULL,
    0u,
    cxta_rolling_min_close_eval,
    NULL,
    cxta_rolling_min_scalar_src,
    NULL,
    NULL,
    NULL,
    cxta_rolling_extrema_params,
    CXTA_ARRAY_COUNT(cxta_rolling_extrema_params),
};
