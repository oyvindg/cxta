/**
 * @file kst.c
 * @brief KST helpers.
 */

#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

#include <cxta/indicators/kst.h>

static const cxta_field_descriptor cxta_kst_fields[] = {
    {"line", offsetof(cxta_kst_output, line), false},
    {"signal", offsetof(cxta_kst_output, signal), false},
    {"histogram", offsetof(cxta_kst_output, histogram), false},
};

static const cxta_plot_field_descriptor cxta_kst_plot_fields[] = {
    {"line", true, "KST Line", "kst", "#22d3ee", "line", "momentum",
     NULL, NULL, NULL, NULL, NULL, NULL, false, false,
     "Weighted sum of four smoothed ROC windows.",
     "Use zero-line behavior, slope, and line/signal crosses to confirm broader momentum turns.",
     false},
    {"signal", true, "KST Signal", "kst", "#f97316", "line", "momentum",
     NULL, NULL, NULL, NULL, NULL, NULL, false, false,
     "SMA-smoothed KST line.",
     "Use as the trigger/reference line for KST momentum crosses.",
     false},
    {"histogram", true, "KST Histogram", "kst", "#a855f7", "histogram", "momentum",
     "#22c55e", "#ef4444", NULL, NULL, NULL, NULL, false, false,
     "Difference between the KST line and signal line.",
     "Expanding bars show increasing momentum spread; zero-line crosses mark line/signal crossovers.",
     false},
};

static const cxta_indicator_plot_descriptor cxta_kst_plot_descriptor = {
    "kst",
    NULL,
    cxta_kst_plot_fields,
    CXTA_ARRAY_COUNT(cxta_kst_plot_fields),
};

static int cxta_kst_descriptor_int_arg(const double* args,
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

static int cxta_kst_descriptor_period_arg(const double* args,
                                          size_t nargs,
                                          size_t index,
                                          int fallback) {
    return cxta_kst_math_clamp_period(
        cxta_kst_descriptor_int_arg(args, nargs, index, fallback));
}

static void cxta_kst_descriptor_eval(const cxta_series_bar_view* view,
                                     const double* args,
                                     size_t nargs,
                                     void* out) {
    cxta_kst_output value;

    if (!out) return;
    value = cxta_kst(
        view,
        cxta_kst_descriptor_period_arg(args, nargs, 0u, 10),
        cxta_kst_descriptor_period_arg(args, nargs, 1u, 15),
        cxta_kst_descriptor_period_arg(args, nargs, 2u, 20),
        cxta_kst_descriptor_period_arg(args, nargs, 3u, 30),
        cxta_kst_descriptor_period_arg(args, nargs, 4u, 9));
    memcpy(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_kst_descriptor = {
    "kst",
    5,
    5,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_kst_output),
    0u,
    cxta_kst_fields,
    CXTA_ARRAY_COUNT(cxta_kst_fields),
    NULL,
    cxta_kst_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_kst_params,
    CXTA_ARRAY_COUNT(cxta_kst_params),
    "kst",
    &cxta_kst_plot_descriptor,
};

cxta_kst_output cxta_kst(const cxta_series_bar_view* view,
                         int p1,
                         int p2,
                         int p3,
                         int p4,
                         int signal_period) {
    cxta_kst_output out = {0.0, 0.0, 0.0};
    size_t idx;

    if (!view || !cxta_series_bar_view_valid(view)) return out;
    idx = cxta_series_clamp_index(view->size, view->index);
    out = cxta_kst_math_eval_rows(
        view->bars,
        view->size,
        idx,
        sizeof(view->bars[0]),
        offsetof(cxta_series_bar, close),
        p1,
        p2,
        p3,
        p4,
        signal_period);
    return out;
}
