/**
 * @file ppo.c
 * @brief Percentage Price Oscillator helpers.
 */

#include <cxta/indicators/ppo.h>
#include <cxta/ts/smoothing.h>
#include <limits.h>
#include <math.h>

static int cxta_ppo_descriptor_int_arg(const double* args,
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

static int cxta_ppo_descriptor_period_arg(const double* args,
                                          size_t nargs,
                                          size_t index,
                                          int fallback) {
    return cxta_ts_clamp_period(cxta_ppo_descriptor_int_arg(args, nargs, index, fallback));
}

static double cxta_ppo_descriptor_eval(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs) {
    int fast = cxta_ppo_descriptor_period_arg(args, nargs, 0u, 12);
    int slow = cxta_ppo_descriptor_period_arg(args, nargs, 1u, 26);
    int swap_tmp;

    if (fast > slow) {
        swap_tmp = fast;
        fast = slow;
        slow = swap_tmp;
    }
    return cxta_ppo(view, fast, slow);
}

const cxta_indicator_descriptor cxta_ppo_descriptor = {
    "ppo",
    2,
    2,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    sizeof(cxta_ppo_state),
    NULL,
    0u,
    cxta_ppo_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_ppo_params,
    CXTA_ARRAY_COUNT(cxta_ppo_params),
};

double cxta_ppo_step(double close, int fast, int slow, cxta_ppo_state* st) {
    if (!st) return 0.0;
    if (fast > slow) {
        const int tmp = fast;
        fast = slow;
        slow = tmp;
    }

    if (st->initialized == 0.0) {
        st->ema_fast = close;
        st->ema_slow = close;
        st->initialized = 1.0;
        return 0.0;
    }

    st->ema_fast = cxta_ts_ema_step(st->ema_fast, close, fast);
    st->ema_slow = cxta_ts_ema_step(st->ema_slow, close, slow);
    if (fabs(st->ema_slow) <= 1e-12) return 0.0;
    return 100.0 * ((st->ema_fast - st->ema_slow) / st->ema_slow);
}

double cxta_ppo(const cxta_series_bar_view* view, int fast, int slow) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    cxta_ppo_state st = {0.0, 0.0, 0.0};
    double out = 0.0;
    for (size_t i = 0; i <= idx; ++i) {
        out = cxta_ppo_step(view->bars[i].close, fast, slow, &st);
    }
    return out;
}
