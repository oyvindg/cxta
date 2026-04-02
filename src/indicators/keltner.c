/**
 * @file keltner.c
 * @brief Keltner channel helpers.
 */

#include <cxta/indicators/keltner.h>
#include <cxta/indicators/atr.h>
#include <cxta/indicators/ema.h>

cxta_channel_output cxta_keltner(const cxta_series_bar_view* view,
                                 int ema_period,
                                 double atr_mult,
                                 int atr_period) {
    cxta_channel_output out = {0.0, 0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    out.middle = cxta_ema(view, ema_period);
    {
        const double atr = cxta_atr(view, atr_period);
        out.upper = out.middle + (atr_mult * atr);
        out.lower = out.middle - (atr_mult * atr);
    }
    out.width = out.upper - out.lower;
    return out;
}
