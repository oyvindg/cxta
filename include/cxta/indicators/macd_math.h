/**
 * @file macd_math.h
 * @brief Pure MACD math shared by host wrappers and CUDA fragments.
 */

#ifndef CXTA_INDICATORS_MACD_MATH_H
#define CXTA_INDICATORS_MACD_MATH_H

#ifndef CXTA_MACD_MATH_FN
#define CXTA_MACD_MATH_FN static inline
#endif

typedef struct {
    double line;
    double signal;
    double histogram;
} cxta_macd_output;

typedef struct {
    double ema_fast;
    double ema_slow;
    double signal;
} cxta_macd_state;

CXTA_MACD_MATH_FN int cxta_macd_math_round_period(double x) {
    const int period = (int)(x >= 0.0 ? x + 0.5 : x - 0.5);
    return period < 1 ? 1 : period;
}

CXTA_MACD_MATH_FN int cxta_macd_math_clamp_period(int period) {
    return period < 1 ? 1 : period;
}

CXTA_MACD_MATH_FN double cxta_macd_math_ema_step(double prev, double x, int period) {
    const int p = cxta_macd_math_clamp_period(period);
    const double a = 2.0 / ((double)p + 1.0);
    return a * x + (1.0 - a) * prev;
}

CXTA_MACD_MATH_FN cxta_macd_state cxta_macd_math_init(double first_close) {
    cxta_macd_state st;
    st.ema_fast = first_close;
    st.ema_slow = first_close;
    st.signal = 0.0;
    return st;
}

CXTA_MACD_MATH_FN cxta_macd_output cxta_macd_math_step(
    double close,
    int fast,
    int slow,
    int signal_period,
    cxta_macd_state* st) {
    cxta_macd_output out;
    out.line = 0.0;
    out.signal = 0.0;
    out.histogram = 0.0;
    if (!st) return out;

    st->ema_fast = cxta_macd_math_ema_step(st->ema_fast, close, fast);
    st->ema_slow = cxta_macd_math_ema_step(st->ema_slow, close, slow);
    out.line = st->ema_fast - st->ema_slow;
    st->signal = cxta_macd_math_ema_step(st->signal, out.line, signal_period);
    out.signal = st->signal;
    out.histogram = out.line - out.signal;
    return out;
}

#endif /* CXTA_INDICATORS_MACD_MATH_H */
