/**
 * @file smoothing.c
 * @brief Core smoothing and range primitives.
 */

#include <cxta/ts/smoothing.h>
#include <cxta/math/math.h>
#include <math.h>

int cxta_ts_clamp_period(int period) {
    return (period < 1) ? 1 : period;
}

double cxta_ts_ema_alpha(int period) {
    return 2.0 / ((double)cxta_ts_clamp_period(period) + 1.0);
}

double cxta_ts_ema_step(double prev, double x, int period) {
    const double a = cxta_ts_ema_alpha(period);
    return a * x + (1.0 - a) * prev;
}

double cxta_ts_wilder_step(double prev, double x, int period) {
    const int p = cxta_ts_clamp_period(period);
    return ((prev * (double)(p - 1)) + x) / (double)p;
}

void cxta_ts_gain_loss(double diff, double* gain, double* loss) {
    if (gain) *gain = (diff > 0.0) ? diff : 0.0;
    if (loss) *loss = (diff < 0.0) ? -diff : 0.0;
}

void cxta_ts_update_gain_loss(double* avg_gain, double* avg_loss, double diff, int period) {
    if (!avg_gain || !avg_loss) return;
    double gain = 0.0;
    double loss = 0.0;
    cxta_ts_gain_loss(diff, &gain, &loss);
    *avg_gain = cxta_ts_wilder_step(*avg_gain, gain, period);
    *avg_loss = cxta_ts_wilder_step(*avg_loss, loss, period);
}

