/**
 * @file adx_math.h
 * @brief Pure ADX math shared by host wrappers and CUDA fragments.
 */

#ifndef CXTA_INDICATORS_ADX_MATH_H
#define CXTA_INDICATORS_ADX_MATH_H

#ifndef CXTA_ADX_MATH_NO_STDLIB
#include <math.h>
#endif

#ifndef CXTA_TS_WILDER_STEP_MATH_H
#include "../ts/wilder_step_math.h"
#endif

#ifndef CXTA_ADX_MATH_FN
#define CXTA_ADX_MATH_FN static inline
#endif

#ifndef CXTA_ADX_MATH_FABS
#define CXTA_ADX_MATH_FABS fabs
#endif

typedef struct {
    double adx;
    double plus_di;
    double minus_di;
} cxta_adx_output;

typedef struct {
    double plus_dm_sm;
    double minus_dm_sm;
    double tr_sm;
    double adx;
} cxta_adx_state;

CXTA_ADX_MATH_FN int cxta_adx_math_round_period(double x) {
    const int period = (int)(x >= 0.0 ? x + 0.5 : x - 0.5);
    return period < 1 ? 1 : period;
}

CXTA_ADX_MATH_FN int cxta_adx_math_clamp_period(int period) {
    return cxta_wilder_step_math_clamp_period(period);
}

CXTA_ADX_MATH_FN double cxta_adx_math_max2(double a, double b) {
    return a > b ? a : b;
}

CXTA_ADX_MATH_FN double cxta_adx_math_true_range(
    double high,
    double low,
    double prev_close) {
    const double h_l = high - low;
    const double h_pc = CXTA_ADX_MATH_FABS(high - prev_close);
    const double l_pc = CXTA_ADX_MATH_FABS(low - prev_close);
    return cxta_adx_math_max2(h_l, cxta_adx_math_max2(h_pc, l_pc));
}

CXTA_ADX_MATH_FN void cxta_adx_math_directional_inputs(
    double high,
    double low,
    double prev_high,
    double prev_low,
    double prev_close,
    double* plus_dm,
    double* minus_dm,
    double* tr) {
    const double up_move = high - prev_high;
    const double down_move = prev_low - low;
    if (plus_dm) {
        *plus_dm = (up_move > down_move && up_move > 0.0) ? up_move : 0.0;
    }
    if (minus_dm) {
        *minus_dm = (down_move > up_move && down_move > 0.0) ? down_move : 0.0;
    }
    if (tr) {
        *tr = cxta_adx_math_true_range(high, low, prev_close);
    }
}

CXTA_ADX_MATH_FN cxta_adx_state cxta_adx_math_init(void) {
    cxta_adx_state st;
    st.plus_dm_sm = 0.0;
    st.minus_dm_sm = 0.0;
    st.tr_sm = 0.0;
    st.adx = 0.0;
    return st;
}

CXTA_ADX_MATH_FN cxta_adx_output cxta_adx_math_step(
    double plus_dm,
    double minus_dm,
    double tr,
    int period,
    cxta_adx_state* st) {
    cxta_adx_output out;
    out.adx = 0.0;
    out.plus_di = 0.0;
    out.minus_di = 0.0;
    if (!st) return out;

    st->plus_dm_sm = cxta_wilder_step_math_step(st->plus_dm_sm, plus_dm, period);
    st->minus_dm_sm = cxta_wilder_step_math_step(st->minus_dm_sm, minus_dm, period);
    st->tr_sm = cxta_wilder_step_math_step(st->tr_sm, tr, period);

    if (st->tr_sm > 1.0e-12) {
        out.plus_di = 100.0 * (st->plus_dm_sm / st->tr_sm);
        out.minus_di = 100.0 * (st->minus_dm_sm / st->tr_sm);
    }

    {
        const double sum = out.plus_di + out.minus_di;
        const double dx = sum > 1.0e-12
            ? 100.0 * CXTA_ADX_MATH_FABS(out.plus_di - out.minus_di) / sum
            : 0.0;
        st->adx = cxta_wilder_step_math_step(st->adx, dx, period);
        out.adx = st->adx;
    }
    return out;
}

CXTA_ADX_MATH_FN cxta_adx_output cxta_adx_math_step_bar(
    double high,
    double low,
    double prev_high,
    double prev_low,
    double prev_close,
    int period,
    cxta_adx_state* st) {
    double plus_dm = 0.0;
    double minus_dm = 0.0;
    double tr = 0.0;
    cxta_adx_math_directional_inputs(
        high,
        low,
        prev_high,
        prev_low,
        prev_close,
        &plus_dm,
        &minus_dm,
        &tr);
    return cxta_adx_math_step(plus_dm, minus_dm, tr, period, st);
}

CXTA_ADX_MATH_FN cxta_adx_output cxta_adx_math_eval_series(
    const double* high,
    const double* low,
    const double* close,
    unsigned long long count,
    unsigned long long index,
    int period) {
    cxta_adx_output out;
    cxta_adx_state st;
    unsigned long long i;
    const int p = cxta_adx_math_clamp_period(period);

    out.adx = 0.0;
    out.plus_di = 0.0;
    out.minus_di = 0.0;
    if (!high || !low || !close || count == 0ull || index == 0ull) return out;
    if (index >= count) index = count - 1ull;

    st = cxta_adx_math_init();
    for (i = 1ull; i <= index; ++i) {
        out = cxta_adx_math_step_bar(
            high[i],
            low[i],
            high[i - 1ull],
            low[i - 1ull],
            close[i - 1ull],
            p,
            &st);
    }
    return out;
}

#endif /* CXTA_INDICATORS_ADX_MATH_H */
