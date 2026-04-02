/**
 * @file rma.c
 * @brief Wilder RMA helpers.
 */

#include <cxta/indicators/rma.h>
#include <cxta/ts/smoothing.h>

double cxta_rma_step(double sample, int period, cxta_rma_state* st) {
    if (!st) return 0.0;
    if (st->initialized == 0.0) {
        st->value = sample;
        st->initialized = 1.0;
        return st->value;
    }
    st->value = cxta_ts_wilder_step(st->value, sample, period);
    return st->value;
}

double cxta_rma(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    cxta_rma_state st = {0.0, 0.0};
    double out = 0.0;
    for (size_t i = 0; i <= idx; ++i) {
        out = cxta_rma_step(view->bars[i].close, period, &st);
    }
    return out;
}
