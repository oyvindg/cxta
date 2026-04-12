/**
 * @file supertrend.c
 * @brief Supertrend helpers.
 */

#include <cxta/indicators/macros.h>
#include <cxta/indicators/supertrend.h>
#include <cxta/math/math.h>
#include <cxta/ts/range.h>
#include <cxta/ts/smoothing.h>
#include <math.h>
#include <stddef.h>

cxta_supertrend_output cxta_supertrend_step(double high,
                                            double low,
                                            double close,
                                            double prev_close,
                                            int period,
                                            double multiplier,
                                            cxta_supertrend_state* st) {
    cxta_supertrend_output out = {0.0, 0.0, 0.0};
    if (!st) return out;

    st->atr = cxta_ts_wilder_step(
        st->atr,
        cxta_ts_true_range(high, low, prev_close),
        period);

    {
        const double hl2 = (high + low) * 0.5;
        const double basic_upper = hl2 + multiplier * st->atr;
        const double basic_lower = hl2 - multiplier * st->atr;

        st->final_upper = (basic_upper < st->final_upper || prev_close > st->final_upper)
            ? basic_upper
            : st->final_upper;
        st->final_lower = (basic_lower > st->final_lower || prev_close < st->final_lower)
            ? basic_lower
            : st->final_lower;
    }

    if (close > st->final_upper) st->direction = 1.0;
    else if (close < st->final_lower) st->direction = -1.0;

    out.value = (st->direction > 0.0) ? st->final_lower : st->final_upper;
    out.direction = st->direction;
    out.strength = fabs(close - out.value) / cxta_math_max2(fabs(close), 1e-12);
    return out;
}

cxta_supertrend_output cxta_supertrend(const cxta_series_bar_view* view,
                                       int period,
                                       double multiplier) {
    cxta_supertrend_output out = {0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    cxta_supertrend_state st = {
        view->bars[0].high - view->bars[0].low,
        0.0,
        0.0,
        1.0,
    };

    for (size_t i = 0; i <= view->index; ++i) {
        if (i == 0) {
            const double hl2 = (view->bars[0].high + view->bars[0].low) * 0.5;
            st.final_upper = hl2 + multiplier * st.atr;
            st.final_lower = hl2 - multiplier * st.atr;
            out.value = view->bars[0].close;
            out.direction = st.direction;
            out.strength = 0.0;
            continue;
        }

        out = cxta_supertrend_step(
            view->bars[i].high,
            view->bars[i].low,
            view->bars[i].close,
            view->bars[i - 1].close,
            period,
            multiplier,
            &st);
    }
    return out;
}

static const cxta_field_descriptor cxta_supertrend_output_fields[] = {
    CXTA_DESC_FIELD("value", offsetof(cxta_supertrend_output, value)),
    CXTA_DESC_FIELD("direction", offsetof(cxta_supertrend_output, direction)),
    CXTA_DESC_FIELD("strength", offsetof(cxta_supertrend_output, strength)),
};

static void cxta_supertrend_desc_eval_struct(const cxta_series_bar_view* view,
                                             const double* args,
                                             size_t nargs,
                                             void* out) {
    const int period = cxta_descriptor_period_arg(args, nargs, 0u, 10);
    const double multiplier = cxta_descriptor_double_arg(args, nargs, 1u, 3.0);
    const cxta_supertrend_output value = cxta_supertrend(view, period, multiplier);
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

const cxta_indicator_descriptor cxta_supertrend_descriptor = {
    "supertrend",
    2,
    2,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_supertrend_output),
    sizeof(cxta_supertrend_state),
    cxta_supertrend_output_fields,
    CXTA_ARRAY_COUNT(cxta_supertrend_output_fields),
    NULL,
    cxta_supertrend_desc_eval_struct,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_supertrend_params,
    CXTA_ARRAY_COUNT(cxta_supertrend_params),
};
