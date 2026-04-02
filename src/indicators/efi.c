/**
 * @file efi.c
 * @brief Elder Force Index helpers.
 */

#include <cxta/indicators/efi.h>
#include <cxta/ts/smoothing.h>

double cxta_efi_step(double close, double volume, int period, cxta_efi_state* st) {
    if (!st) return 0.0;
    if (st->initialized == 0.0) {
        st->prev_close = close;
        st->initialized = 1.0;
        return 0.0;
    }

    {
        const double raw = (close - st->prev_close) * volume;
        if (period <= 1 || st->seeded == 0.0) {
            st->value = raw;
            st->seeded = 1.0;
        } else {
            st->value = cxta_ts_ema_step(st->value, raw, period);
        }
        st->prev_close = close;
        return st->value;
    }
}

double cxta_efi(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    cxta_efi_state st = {0.0, 0.0, 0.0, 0.0};
    double out = 0.0;
    for (size_t i = 0; i <= idx; ++i) {
        out = cxta_efi_step(view->bars[i].close, view->bars[i].volume, period, &st);
    }
    return out;
}
