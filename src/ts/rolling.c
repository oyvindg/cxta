/**
 * @file rolling.c
 * @brief Rolling mean/variance helpers.
 */

#include <cxta/ts/rolling.h>

void cxta_ts_rolling_reset(cxta_ts_rolling_state* st) {
    if (!st) return;
    st->sum = 0.0;
    st->sum_sq = 0.0;
    st->count = 0;
}

void cxta_ts_rolling_add(cxta_ts_rolling_state* st, double x) {
    if (!st) return;
    st->sum += x;
    st->sum_sq += x * x;
    st->count += 1;
}

void cxta_ts_rolling_remove(cxta_ts_rolling_state* st, double x) {
    if (!st || st->count == 0) return;
    st->sum -= x;
    st->sum_sq -= x * x;
    st->count -= 1;
}

double cxta_ts_rolling_mean(const cxta_ts_rolling_state* st) {
    if (!st || st->count == 0) return 0.0;
    return st->sum / (double)st->count;
}

double cxta_ts_rolling_var(const cxta_ts_rolling_state* st) {
    if (!st || st->count == 0) return 0.0;
    const double m = cxta_ts_rolling_mean(st);
    const double v = (st->sum_sq / (double)st->count) - (m * m);
    return (v < 0.0) ? 0.0 : v;
}
