/**
 * @file liquidity.c
 * @brief Liquidity zone helpers derived from pivot state.
 */

#include <cxta/structure/liquidity.h>
#include <math.h>

static void cxta_struct_liquidity_state_reset(cxta_struct_liquidity_state* out) {
    out->buy_side = 0.0;
    out->sell_side = 0.0;
    out->level = 0.0;
    out->swept = 0.0;
}

static int cxta_struct_is_equal_level(double a, double b, double tolerance) {
    const double scale = fmax(fmax(fabs(a), fabs(b)), 1e-12);
    return (fabs(a - b) / scale) <= tolerance;
}

int cxta_struct_liquidity_state_compute(const cxta_series_bar_view*   view,
                                const cxta_struct_pivot_state* pivot,
                                double                tolerance,
                                cxta_struct_liquidity_state*  out) {
    if (!view || !pivot || !out) return 0;
    cxta_struct_liquidity_state_reset(out);
    if (!cxta_series_bar_view_valid(view)) return 1;

    double tol = fabs(tolerance);
    if (!isfinite(tol)) tol = 0.001;
    if (tol < 1e-9) tol = 1e-9;

    double buy_level = 0.0;
    double sell_level = 0.0;

    if (pivot->has_prev_high && pivot->has_last_high &&
        cxta_struct_is_equal_level(pivot->high, pivot->prev_high, tol)) {
        out->buy_side = 1.0;
        buy_level = 0.5 * (pivot->high + pivot->prev_high);
    }

    if (pivot->has_prev_low && pivot->has_last_low &&
        cxta_struct_is_equal_level(pivot->low, pivot->prev_low, tol)) {
        out->sell_side = 1.0;
        sell_level = 0.5 * (pivot->low + pivot->prev_low);
    }

    const cxta_series_bar* bar = cxta_series_bar_view_current(view);
    if (!bar) return 1;

    if (out->buy_side > 0.5 && out->sell_side > 0.5) {
        const double d_buy = fabs(bar->close - buy_level);
        const double d_sell = fabs(bar->close - sell_level);
        if (d_buy <= d_sell) {
            out->sell_side = 0.0;
        } else {
            out->buy_side = 0.0;
        }
    }

    if (out->buy_side > 0.5) {
        out->level = buy_level;
        if (bar->high > out->level && bar->close < out->level) {
            out->swept = 1.0;
        }
    } else if (out->sell_side > 0.5) {
        out->level = sell_level;
        if (bar->low < out->level && bar->close > out->level) {
            out->swept = 1.0;
        }
    }

    return 1;
}
