/**
 * @file rsi.c
 * @brief Relative Strength Index helpers.
 */

#include <limits.h>
#include <math.h>

#include <cxta/indicators/rsi.h>
#include <cxta/ts/smoothing.h>

typedef struct {
    double avg_gain;
    double avg_loss;
    double prev_value;
    double samples_seen;
} cxta_rsi_descriptor_state;

static const cxta_scalar_plot_descriptor cxta_rsi_scalar_plot = {
    .auto_plot = true,
    .label = "RSI",
    .pane = "rsi",
    .color = "#22d3ee",
    .style = "line",
    .scale = "rsi",
    .hover_summary = "Relative Strength Index oscillator.",
    .hover_indication = "Use overbought/oversold zones, center-line behavior, and divergences rather than raw value alone.",
};

const cxta_indicator_plot_descriptor cxta_rsi_plot_descriptor = {
    "rsi",
    &cxta_rsi_scalar_plot,
    NULL,
    0u,
};

static double cxta_rsi_from_averages(double avg_gain, double avg_loss);

static int cxta_rsi_descriptor_period_arg(const double* args,
                                          size_t nargs,
                                          size_t index,
                                          int fallback) {
    double raw;

    if (!args || index >= nargs) return fallback;
    raw = args[index];
    if (!isfinite(raw)) return fallback;
    if (raw >= (double)INT_MAX) return INT_MAX;
    if (raw <= (double)INT_MIN) return INT_MIN;
    return cxta_ts_clamp_period((int)llround(raw));
}

static double cxta_rsi_descriptor_eval(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs) {
    return cxta_rsi(view, cxta_rsi_descriptor_period_arg(args, nargs, 0u, 14));
}

static double cxta_rsi_descriptor_step(double close,
                                       double prev_close,
                                       const double* args,
                                       size_t nargs,
                                       void* state) {
    cxta_rsi_descriptor_state* st = (cxta_rsi_descriptor_state*)state;
    const int period = cxta_rsi_descriptor_period_arg(args, nargs, 0u, 14);
    double diff;

    (void)prev_close;
    if (!st) return 50.0;

    if (st->samples_seen <= 0.0) {
        st->prev_value = close;
        st->samples_seen = 1.0;
        return 50.0;
    }

    diff = close - st->prev_value;
    if (st->samples_seen <= (double)period) {
        if (diff > 0.0) st->avg_gain += diff;
        else st->avg_loss -= diff;
        st->prev_value = close;
        st->samples_seen += 1.0;

        if (st->samples_seen <= (double)period) return 50.0;

        st->avg_gain /= (double)period;
        st->avg_loss /= (double)period;
        return cxta_rsi_from_averages(st->avg_gain, st->avg_loss);
    }

    cxta_ts_update_gain_loss(&st->avg_gain, &st->avg_loss, diff, period);
    st->prev_value = close;
    return cxta_rsi_from_averages(st->avg_gain, st->avg_loss);
}

static double cxta_rsi_descriptor_eval_scalar_src(const cxta_series_scalar_view* source,
                                                  const double* args,
                                                  size_t nargs) {
    size_t idx;
    size_t i;
    int period;
    double avg_gain = 0.0;
    double avg_loss = 0.0;

    if (!source || !cxta_series_scalar_view_valid(source)) return 50.0;
    idx = cxta_series_clamp_index(source->size, source->index);
    period = cxta_rsi_descriptor_period_arg(args, nargs, 0u, 14);
    if (idx == 0u || idx < (size_t)period) return 50.0;

    for (i = 1u; i <= (size_t)period; ++i) {
        double gain = 0.0;
        double loss = 0.0;
        cxta_ts_gain_loss(source->values[i] - source->values[i - 1u], &gain, &loss);
        avg_gain += gain;
        avg_loss += loss;
    }
    avg_gain /= (double)period;
    avg_loss /= (double)period;

    for (i = (size_t)period + 1u; i <= idx; ++i) {
        cxta_ts_update_gain_loss(
            &avg_gain,
            &avg_loss,
            source->values[i] - source->values[i - 1u],
            period);
    }
    return cxta_rsi_from_averages(avg_gain, avg_loss);
}

const cxta_indicator_descriptor cxta_rsi_descriptor = {
    "rsi",
    1,
    1,
    1,
    1,
    -1,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_SCALAR_SOURCE,
    0u,
    sizeof(cxta_rsi_descriptor_state),
    NULL,
    0u,
    cxta_rsi_descriptor_eval,
    NULL,
    cxta_rsi_descriptor_eval_scalar_src,
    NULL,
    cxta_rsi_descriptor_step,
    NULL,
    cxta_rsi_params,
    CXTA_ARRAY_COUNT(cxta_rsi_params),
    "rsi",
    &cxta_rsi_plot_descriptor,
};

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
