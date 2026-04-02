/**
 * @file supertrend.c
 * @brief Supertrend helpers.
 */

#include <cxta/indicators/supertrend.h>
#include <cxta/math/math.h>
#include <cxta/ts/range.h>
#include <cxta/ts/smoothing.h>
#include <math.h>

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
