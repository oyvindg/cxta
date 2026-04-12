/**
 * @file nvi.c
 * @brief Negative and Positive Volume Index helpers.
 */

#include <cxta/indicators/nvi.h>
#include <math.h>

static double cxta_volume_index_step(double close,
                                     double volume,
                                     cxta_nvi_state* st,
                                     int want_lower_volume) {
    if (!st) return 0.0;
    if (st->initialized == 0.0) {
        st->value = 1000.0;
        st->prev_close = close;
        st->prev_volume = volume;
        st->initialized = 1.0;
        return st->value;
    }

    {
        const int volume_match = want_lower_volume
            ? (volume < st->prev_volume)
            : (volume > st->prev_volume);
        if (volume_match && fabs(st->prev_close) > 1e-12) {
            st->value += st->value * ((close - st->prev_close) / st->prev_close);
        }
        st->prev_close = close;
        st->prev_volume = volume;
        return st->value;
    }
}

double cxta_nvi_step(double close, double volume, cxta_nvi_state* st) {
    return cxta_volume_index_step(close, volume, st, 1);
}

double cxta_pvi_step(double close, double volume, cxta_nvi_state* st) {
    return cxta_volume_index_step(close, volume, st, 0);
}

double cxta_nvi(const cxta_series_bar_view* view) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    cxta_nvi_state st = {0.0, 0.0, 0.0, 0.0};
    double out = 0.0;
    for (size_t i = 0; i <= idx; ++i) {
        out = cxta_nvi_step(view->bars[i].close, view->bars[i].volume, &st);
    }
    return out;
}

double cxta_pvi(const cxta_series_bar_view* view) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    cxta_nvi_state st = {0.0, 0.0, 0.0, 0.0};
    double out = 0.0;
    for (size_t i = 0; i <= idx; ++i) {
        out = cxta_pvi_step(view->bars[i].close, view->bars[i].volume, &st);
    }
    return out;
}

const cxta_bridge_fn_spec cxta_nvi_bridge_fn_spec = {
    "nvi",
    0u,
    0u,
    NULL,
    0u,
    NULL,
    0u,
    1,
};

const cxta_bridge_fn_spec cxta_pvi_bridge_fn_spec = {
    "pvi",
    0u,
    0u,
    NULL,
    0u,
    NULL,
    0u,
    1,
};

static double cxta_nvi_descriptor_eval(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs) {
    (void)args;
    (void)nargs;
    return cxta_nvi(view);
}

static double cxta_pvi_descriptor_eval(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs) {
    (void)args;
    (void)nargs;
    return cxta_pvi(view);
}

const cxta_indicator_descriptor cxta_nvi_descriptor = {
    "nvi",
    0,
    0,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    sizeof(cxta_nvi_state),
    NULL,
    0u,
    cxta_nvi_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0u,
};

const cxta_indicator_descriptor cxta_pvi_descriptor = {
    "pvi",
    0,
    0,
    -1,
    -1,
    -1,
    CXTA_INDICATOR_SCALAR,
    0u,
    sizeof(cxta_nvi_state),
    NULL,
    0u,
    cxta_pvi_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0u,
};
