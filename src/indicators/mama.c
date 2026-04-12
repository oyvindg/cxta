/**
 * @file mama.c
 * @brief MAMA/FAMA helpers.
 */

#include <cxta/indicators/mama.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

static const cxta_field_descriptor cxta_mama_fields[] = {
    {"mama", offsetof(cxta_mama_output, mama), true},
    {"fama", offsetof(cxta_mama_output, fama), true},
};

static double cxta_mama_descriptor_double_arg(const double* args,
                                            size_t nargs,
                                            size_t index,
                                            double fallback) {
    double raw;

    if (!args || index >= nargs) return fallback;
    raw = args[index];
    return isfinite(raw) ? raw : fallback;
}

static void cxta_mama_descriptor_eval(const cxta_series_bar_view* view,
                                      const double* args,
                                      size_t nargs,
                                      void* out) {
    double fast_limit = cxta_mama_descriptor_double_arg(args, nargs, 0u, 0.5);
    double slow_limit = cxta_mama_descriptor_double_arg(args, nargs, 1u, 0.05);
    cxta_mama_output value;

    if (fast_limit < 0.01) fast_limit = 0.01;
    if (fast_limit > 0.99) fast_limit = 0.99;
    if (slow_limit < 0.001) slow_limit = 0.001;
    if (slow_limit > fast_limit) slow_limit = fast_limit;
    value = cxta_mama(view, fast_limit, slow_limit);
    if (out) memcpy(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_mama_descriptor = {
    "mama",
    0,
    2,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_mama_output),
    0u,
    cxta_mama_fields,
    CXTA_ARRAY_COUNT(cxta_mama_fields),
    NULL,
    cxta_mama_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_mama_params,
    CXTA_ARRAY_COUNT(cxta_mama_params),
};

static double cxta_mama_clamp(double x, double lo, double hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

cxta_mama_output cxta_mama(const cxta_series_bar_view* view,
                           double fast_limit,
                           double slow_limit) {
    cxta_mama_output out = {0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    fast_limit = cxta_mama_clamp(fast_limit, 0.01, 0.99);
    slow_limit = cxta_mama_clamp(slow_limit, 0.001, fast_limit);
    if (slow_limit > fast_limit) {
        const double tmp = slow_limit;
        slow_limit = fast_limit;
        fast_limit = tmp;
    }

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        double mama = view->bars[0].close;
        double fama = mama;

        for (size_t i = 1; i <= idx; ++i) {
            const size_t back = (i > 10u) ? (i - 10u) : 0u;
            double change = view->bars[i].close - view->bars[back].close;
            if (change < 0.0) change = -change;

            {
                double volatility = 0.0;
                for (size_t j = back + 1; j <= i; ++j) {
                    double delta = view->bars[j].close - view->bars[j - 1].close;
                    if (delta < 0.0) delta = -delta;
                    volatility += delta;
                }

                {
                    const double er = (volatility <= 1e-12) ? 0.0 : (change / volatility);
                    const double alpha = cxta_mama_clamp(
                        slow_limit + (fast_limit - slow_limit) * er,
                        slow_limit,
                        fast_limit);
                    const double fama_alpha = cxta_mama_clamp(
                        0.5 * alpha,
                        slow_limit * 0.5,
                        fast_limit * 0.5);
                    mama = alpha * view->bars[i].close + (1.0 - alpha) * mama;
                    fama = fama_alpha * mama + (1.0 - fama_alpha) * fama;
                }
            }
        }

        out.mama = mama;
        out.fama = fama;
    }
    return out;
}
