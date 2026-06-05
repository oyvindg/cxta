/**
 * @file args.c
 * @brief Descriptor argument helpers for expression-facing indicators.
 */

#include <cxta/cxta.h>

#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

int cxta_descriptor_int_arg(const double* args,
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

int cxta_descriptor_period_arg(const double* args,
                               size_t nargs,
                               size_t index,
                               int fallback) {
    return cxta_ts_clamp_period(cxta_descriptor_int_arg(args, nargs, index, fallback));
}

int cxta_descriptor_clamp_int_arg(const double* args,
                                  size_t nargs,
                                  size_t index,
                                  int fallback,
                                  int min_value,
                                  int max_value) {
    int value = cxta_descriptor_int_arg(args, nargs, index, fallback);
    if (value < min_value) value = min_value;
    if (value > max_value) value = max_value;
    return value;
}

double cxta_descriptor_double_arg(const double* args,
                                  size_t nargs,
                                  size_t index,
                                  double fallback) {
    double raw;

    if (!args || index >= nargs) return fallback;
    raw = args[index];
    return isfinite(raw) ? raw : fallback;
}

int cxta_descriptor_parse_pivot_args(const cxta_series_bar_view* view,
                                     const double* args,
                                     size_t nargs,
                                     cxta_struct_pivot_args* out) {
    const size_t current_index =
        (view && view->size > 0u) ? cxta_series_clamp_index(view->size, view->index) : 0u;
    return cxta_struct_pivot_args_parse(args, nargs, current_index, out);
}

void cxta_descriptor_copy_struct(void* out, const void* value, size_t size) {
    if (!out || !value || size == 0u) return;
    memcpy(out, value, size);
}
