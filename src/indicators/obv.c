/**
 * @file obv.c
 * @brief On Balance Volume helpers.
 */

#include <cxta/indicators/macros.h>
#include <cxta/indicators/obv.h>

static const cxta_scalar_plot_descriptor cxta_obv_scalar_plot =
    CXTA_SCALAR_PLOT("OBV", "volume", "#38bdf8", "line", "volume",
                     "On Balance Volume cumulative flow line.",
                     "Use divergence and trend in OBV to confirm or question price moves.");

static const cxta_indicator_plot_descriptor cxta_obv_plot_descriptor =
    CXTA_INDICATOR_SCALAR_PLOT("obv", cxta_obv_scalar_plot);

double cxta_obv_step(double close, double volume, cxta_obv_state* st) {
    if (!st) return 0.0;
    if (st->initialized == 0.0) {
        st->prev_close = close;
        st->initialized = 1.0;
        return st->value;
    }
    if (close > st->prev_close)      st->value += volume;
    else if (close < st->prev_close) st->value -= volume;
    st->prev_close = close;
    return st->value;
}

double cxta_obv(const cxta_series_bar_view* view) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    cxta_obv_state st = {0.0, 0.0, 0.0};
    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    for (size_t i = 0; i <= idx; ++i) {
        cxta_obv_step(view->bars[i].close, view->bars[i].volume, &st);
    }
    return st.value;
}

CXTA_WRAP_BAR_SCALAR_0(cxta_obv_desc_eval, cxta_obv)

const cxta_bridge_fn_spec cxta_obv_bridge_fn_spec = {
    "obv",
    0u,
    0u,
    NULL,
    0u,
    NULL,
    0u,
    1,
};

const cxta_indicator_descriptor cxta_obv_descriptor = {
    "obv",
    0,
    0,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    sizeof(cxta_obv_state),
    NULL,
    0u,
    cxta_obv_desc_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0u,
    "volume",
    &cxta_obv_plot_descriptor,
};
