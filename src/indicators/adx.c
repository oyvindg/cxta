/**
 * @file adx.c
 * @brief ADX helpers.
 */

#include <cxta/indicators/adx.h>
#include <cxta/ts/range.h>
#include <cxta/ts/smoothing.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

static const cxta_field_descriptor cxta_adx_fields[] = {
    {"adx", offsetof(cxta_adx_output, adx), true},
    {"plusDI", offsetof(cxta_adx_output, plus_di), true},
    {"minusDI", offsetof(cxta_adx_output, minus_di), true},
};

static int cxta_adx_descriptor_int_arg(const double* args,
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

static int cxta_adx_descriptor_period_arg(const double* args,
                                          size_t nargs,
                                          size_t index,
                                          int fallback) {
    return cxta_ts_clamp_period(cxta_adx_descriptor_int_arg(args, nargs, index, fallback));
}

static void cxta_adx_descriptor_eval(const cxta_series_bar_view* view,
                                     const double* args,
                                     size_t nargs,
                                     void* out) {
    cxta_adx_output value;
    memset(&value, 0, sizeof(value));
    value = cxta_adx(view, cxta_adx_descriptor_period_arg(args, nargs, 0u, 14));
    if (out) memcpy(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_adx_descriptor = {
    "adx",
    1,
    1,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_adx_output),
    sizeof(cxta_adx_state),
    cxta_adx_fields,
    CXTA_ARRAY_COUNT(cxta_adx_fields),
    NULL,
    cxta_adx_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_adx_params,
    CXTA_ARRAY_COUNT(cxta_adx_params),
};

cxta_adx_output cxta_adx_step(double plus_dm,
                              double minus_dm,
                              double tr,
                              int period,
                              cxta_adx_state* st) {
    cxta_adx_output out = {0.0, 0.0, 0.0};
    if (!st) return out;

    st->plus_dm_sm = cxta_ts_wilder_step(st->plus_dm_sm, plus_dm, period);
    st->minus_dm_sm = cxta_ts_wilder_step(st->minus_dm_sm, minus_dm, period);
    st->tr_sm = cxta_ts_wilder_step(st->tr_sm, tr, period);

    if (st->tr_sm > 1e-12) {
        out.plus_di = 100.0 * (st->plus_dm_sm / st->tr_sm);
        out.minus_di = 100.0 * (st->minus_dm_sm / st->tr_sm);
    }

    {
        const double sum = out.plus_di + out.minus_di;
        const double dx = (sum > 1e-12)
            ? (100.0 * fabs(out.plus_di - out.minus_di) / sum)
            : 0.0;
        st->adx = cxta_ts_wilder_step(st->adx, dx, period);
        out.adx = st->adx;
    }
    return out;
}

cxta_adx_output cxta_adx(const cxta_series_bar_view* view, int period) {
    cxta_adx_output out = {0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view) || view->index == 0) return out;

    const int p = cxta_ts_clamp_period(period);
    cxta_adx_state st = {0.0, 0.0, 0.0, 0.0};
    for (size_t i = 1; i <= view->index; ++i) {
        const double up_move = view->bars[i].high - view->bars[i - 1].high;
        const double down_move = view->bars[i - 1].low - view->bars[i].low;
        const double plus_dm = (up_move > down_move && up_move > 0.0) ? up_move : 0.0;
        const double minus_dm = (down_move > up_move && down_move > 0.0) ? down_move : 0.0;
        out = cxta_adx_step(
            plus_dm,
            minus_dm,
            cxta_ts_true_range(view->bars[i].high, view->bars[i].low, view->bars[i - 1].close),
            p,
            &st);
    }
    return out;
}
