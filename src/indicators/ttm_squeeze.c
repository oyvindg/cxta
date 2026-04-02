/**
 * @file ttm_squeeze.c
 * @brief TTM squeeze helpers.
 */

#include <cxta/indicators/ttm_squeeze.h>
#include <cxta/indicators/bollinger.h>
#include <cxta/indicators/keltner.h>
#include <cxta/indicators/sma.h>

cxta_squeeze_output cxta_ttm_squeeze(const cxta_series_bar_view* view,
                                     int bb_period,
                                     double bb_mult,
                                     int kc_period,
                                     double kc_mult,
                                     int momentum_period) {
    cxta_squeeze_output out = {0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    {
        const cxta_bollinger_output bb = cxta_bollinger(view, bb_period, bb_mult);
        const cxta_channel_output kc = cxta_keltner(view, kc_period, kc_mult, kc_period);
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const double momentum_mean = cxta_sma(view, momentum_period);

        out.squeeze_on = (bb.upper < kc.upper && bb.lower > kc.lower) ? 1.0 : 0.0;
        out.squeeze_off = (bb.upper > kc.upper && bb.lower < kc.lower) ? 1.0 : 0.0;
        out.momentum = view->bars[idx].close - momentum_mean;
    }
    return out;
}
