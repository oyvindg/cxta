/**
 * @file macros.h
 * @brief Shared macros for building expression `eval_scalar` / `eval_struct` wrappers in indicator TUs.
 */

#pragma once

#include <stddef.h>

#include <cxta/indicators/descriptor.h>

/** @brief One output field for struct indicators (all auto-plot). */
#define CXTA_DESC_FIELD(name, offset) {(name), (offset), true}

#define CXTA_WRAP_BAR_SCALAR_0(wrapper, fn)                                                   \
    static double wrapper(const cxta_series_bar_view* view, const double* args, size_t nargs) { \
        (void)args;                                                                             \
        (void)nargs;                                                                            \
        return fn(view);                                                                        \
    }

#define CXTA_WRAP_BAR_SCALAR_1I(wrapper, fn, default0)                                        \
    static double wrapper(const cxta_series_bar_view* view, const double* args, size_t nargs) { \
        return fn(view, cxta_descriptor_period_arg(args, nargs, 0u, (default0)));               \
    }

#define CXTA_WRAP_BAR_SCALAR_2I(wrapper, fn, default0, default1)                              \
    static double wrapper(const cxta_series_bar_view* view, const double* args, size_t nargs) { \
        return fn(view,                                                                       \
                  cxta_descriptor_period_arg(args, nargs, 0u, (default0)),                    \
                  cxta_descriptor_period_arg(args, nargs, 1u, (default1)));                   \
    }

#define CXTA_WRAP_BAR_SCALAR_3I(wrapper, fn, default0, default1, default2)                     \
    static double wrapper(const cxta_series_bar_view* view, const double* args, size_t nargs) { \
        return fn(view,                                                                       \
                  cxta_descriptor_period_arg(args, nargs, 0u, (default0)),                    \
                  cxta_descriptor_period_arg(args, nargs, 1u, (default1)),                    \
                  cxta_descriptor_period_arg(args, nargs, 2u, (default2)));                     \
    }

#define CXTA_WRAP_BAR_SCALAR_4I(wrapper, fn, default0, default1, default2, default3)          \
    static double wrapper(const cxta_series_bar_view* view, const double* args, size_t nargs) { \
        return fn(view,                                                                       \
                  cxta_descriptor_period_arg(args, nargs, 0u, (default0)),                    \
                  cxta_descriptor_period_arg(args, nargs, 1u, (default1)),                    \
                  cxta_descriptor_period_arg(args, nargs, 2u, (default2)),                    \
                  cxta_descriptor_period_arg(args, nargs, 3u, (default3)));                     \
    }

#define CXTA_WRAP_BAR_STRUCT_2D(wrapper, type, fn, default0, default1)                        \
    static void wrapper(const cxta_series_bar_view* view,                                     \
                        const double* args,                                                   \
                        size_t nargs,                                                         \
                        void* out) {                                                          \
        type value;                                                                           \
        memset(&value, 0, sizeof(value));                                                     \
        value = fn(view,                                                                      \
                   cxta_descriptor_double_arg(args, nargs, 0u, (default0)),                   \
                   cxta_descriptor_double_arg(args, nargs, 1u, (default1)));                    \
        cxta_descriptor_copy_struct(out, &value, sizeof(value));                              \
    }
