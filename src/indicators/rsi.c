/**
 * @file rsi.c
 * @brief Relative Strength Index helpers.
 */

#include <cxta/indicators/rsi.h>
#include <cxta/ts/smoothing.h>

static double cxta_rsi_from_averages(double avg_gain, double avg_loss) {
    if (avg_loss < 1e-12) return 100.0;
    return 100.0 - (100.0 / (1.0 + (avg_gain / avg_loss)));
}

void cxta_rsi_seed(const cxta_series_bar_view* view, int period, cxta_rsi_state* st) {
    if (!st) return;
    st->avg_gain = 0.0;
    st->avg_loss = 0.0;
    if (!view || !cxta_series_bar_view_valid(view)) return;

    const int p = cxta_ts_clamp_period(period);
    if (view->size <= (size_t)p) return;
    for (size_t i = 1; i <= (size_t)p; ++i) {
        double gain = 0.0;
        double loss = 0.0;
        cxta_ts_gain_loss(view->bars[i].close - view->bars[i - 1].close, &gain, &loss);
        st->avg_gain += gain;
        st->avg_loss += loss;
    }
    st->avg_gain /= (double)p;
    st->avg_loss /= (double)p;
}

double cxta_rsi_step(double close, double prev_close, int period, cxta_rsi_state* st) {
    if (!st) return 50.0;
    cxta_ts_update_gain_loss(&st->avg_gain, &st->avg_loss, close - prev_close, period);
    return cxta_rsi_from_averages(st->avg_gain, st->avg_loss);
}

double cxta_rsi(const cxta_series_bar_view* view, int period) {
    if (!view || !cxta_series_bar_view_valid(view)) return 50.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const int p = cxta_ts_clamp_period(period);
    if (idx == 0 || idx < (size_t)p) return 50.0;

    cxta_rsi_state st;
    cxta_rsi_seed(view, p, &st);
    for (size_t i = (size_t)p + 1; i <= idx; ++i) {
        cxta_ts_update_gain_loss(
            &st.avg_gain,
            &st.avg_loss,
            view->bars[i].close - view->bars[i - 1].close,
            p);
    }
    return cxta_rsi_from_averages(st.avg_gain, st.avg_loss);
}
