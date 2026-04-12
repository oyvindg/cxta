/**
 * @file vwap.c
 * @brief Swing-anchor VWAP helpers implementation.
 */

#include <cxta/structure/vwap.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

static double cxta_struct_vwap_true_range_at(const cxta_series_bar_view* view, size_t index) {
    const cxta_series_bar* bar = cxta_series_bar_view_at(view, index);
    if (!bar) return 0.0;
    if (index == 0) return bar->high - bar->low;

    const cxta_series_bar* prev = cxta_series_bar_view_at(view, index - 1);
    if (!prev) return bar->high - bar->low;

    const double hl = bar->high - bar->low;
    const double hc = fabs(bar->high - prev->close);
    const double lc = fabs(bar->low - prev->close);
    double tr = hl;
    if (hc > tr) tr = hc;
    if (lc > tr) tr = lc;
    return tr;
}

static double cxta_struct_vwap_alpha_from_apt(double apt) {
    const double clamped = (apt < 1.0) ? 1.0 : apt;
    const double decay = exp(-log(2.0) / clamped);
    return 1.0 - decay;
}

static double cxta_struct_vwap_alpha_for_args(const cxta_struct_vwap_args* args,
                                        double atr,
                                        double atr_avg) {
    double ratio = (fabs(atr_avg) > 1e-12) ? (atr / atr_avg) : 1.0;
    if (!isfinite(ratio) || ratio <= 0.0) ratio = 1.0;

    double apt = args->apt;
    if (args->use_adapt) {
        apt = args->apt / pow(ratio, args->vol_bias);
    }
    if (!isfinite(apt)) apt = args->apt;
    if (apt < args->min_apt) apt = args->min_apt;
    if (apt > args->max_apt) apt = args->max_apt;
    return cxta_struct_vwap_alpha_from_apt(round(apt));
}

static int cxta_struct_vwap_is_window_high(const cxta_series_bar_view* view,
                                     size_t start,
                                     size_t end) {
    const cxta_series_bar* current = cxta_series_bar_view_at(view, end);
    if (!current) return 0;
    for (size_t i = start; i <= end; ++i) {
        const cxta_series_bar* bar = cxta_series_bar_view_at(view, i);
        if (!bar) return 0;
        if (bar->high > current->high) return 0;
    }
    return 1;
}

static int cxta_struct_vwap_is_window_low(const cxta_series_bar_view* view,
                                    size_t start,
                                    size_t end) {
    const cxta_series_bar* current = cxta_series_bar_view_at(view, end);
    if (!current) return 0;
    for (size_t i = start; i <= end; ++i) {
        const cxta_series_bar* bar = cxta_series_bar_view_at(view, i);
        if (!bar) return 0;
        if (bar->low < current->low) return 0;
    }
    return 1;
}

static void cxta_struct_vwap_replay_from_anchor(const cxta_series_bar_view* view,
                                          const cxta_struct_vwap_args* args,
                                          size_t anchor_index,
                                          size_t target_index,
                                          double anchor_price,
                                          double anchor_atr,
                                          double anchor_atr_avg,
                                          double* p_acc,
                                          double* v_acc) {
    const cxta_series_bar* anchor_bar = cxta_series_bar_view_at(view, anchor_index);
    if (!anchor_bar || !p_acc || !v_acc) return;

    *p_acc = anchor_price * anchor_bar->volume;
    *v_acc = anchor_bar->volume;

    double alpha = cxta_struct_vwap_alpha_for_args(args, anchor_atr, anchor_atr_avg);
    double pxv = cxta_series_typical_price(anchor_bar) * anchor_bar->volume;
    *p_acc = (1.0 - alpha) * (*p_acc) + alpha * pxv;
    *v_acc = (1.0 - alpha) * (*v_acc) + alpha * anchor_bar->volume;

    double atr = anchor_atr;
    double atr_avg = anchor_atr_avg;
    for (size_t j = anchor_index + 1; j <= target_index; ++j) {
        const cxta_series_bar* bar = cxta_series_bar_view_at(view, j);
        if (!bar) continue;
        const double tr = cxta_struct_vwap_true_range_at(view, j);
        const double period = (double)((args->atr_period < 1) ? 1 : args->atr_period);
        atr = ((atr * (period - 1.0)) + tr) / period;
        atr_avg = ((atr_avg * (period - 1.0)) + atr) / period;
        alpha = cxta_struct_vwap_alpha_for_args(args, atr, atr_avg);
        pxv = cxta_series_typical_price(bar) * bar->volume;
        *p_acc = (1.0 - alpha) * (*p_acc) + alpha * pxv;
        *v_acc = (1.0 - alpha) * (*v_acc) + alpha * bar->volume;
    }
}

int cxta_struct_vwap_args_parse(const double* args,
                                     size_t nargs,
                                     cxta_struct_vwap_args* out) {
    if (!out) return 0;

    out->swing_period = CXTA_STRUCT_VWAP_DEFAULT_SWING_PERIOD;
    out->apt = CXTA_STRUCT_VWAP_DEFAULT_APT;
    out->use_adapt = CXTA_STRUCT_VWAP_DEFAULT_USE_ADAPT;
    out->vol_bias = CXTA_STRUCT_VWAP_DEFAULT_VOL_BIAS;
    out->atr_period = CXTA_STRUCT_VWAP_DEFAULT_ATR_PERIOD;
    out->min_apt = CXTA_STRUCT_VWAP_DEFAULT_MIN_APT;
    out->max_apt = CXTA_STRUCT_VWAP_DEFAULT_MAX_APT;
    if (!args || nargs == 0) return 1;

    if (nargs >= 1 && isfinite(args[0])) {
        const int swing_period = (int)llround(args[0]);
        out->swing_period = (swing_period < 2) ? 2 : swing_period;
    }
    if (nargs >= 2 && isfinite(args[1])) {
        out->apt = (args[1] < 1.0) ? 1.0 : args[1];
    }
    if (nargs >= 3 && isfinite(args[2])) {
        out->use_adapt = (llround(args[2]) != 0) ? 1 : 0;
    }
    if (nargs >= 4 && isfinite(args[3])) {
        out->vol_bias = (args[3] < 0.0) ? 0.0 : args[3];
    }
    if (nargs >= 5 && isfinite(args[4])) {
        const int atr_period = (int)llround(args[4]);
        out->atr_period = (atr_period < 1) ? 1 : atr_period;
    }
    if (nargs >= 6 && isfinite(args[5])) {
        out->min_apt = (args[5] < 1.0) ? 1.0 : args[5];
    }
    if (nargs >= 7 && isfinite(args[6])) {
        out->max_apt = (args[6] < out->min_apt) ? out->min_apt : args[6];
    }
    if (out->max_apt < out->min_apt) out->max_apt = out->min_apt;
    return 1;
}

int cxta_struct_vwap_compute(const cxta_series_bar_view* view,
                                  const cxta_struct_vwap_args* args,
                                  cxta_struct_vwap_result* out) {
    cxta_struct_vwap_state state;

    if (!out) return 0;

    memset(out, 0, sizeof(*out));
    if (!view || !args || !cxta_series_bar_view_valid(view)) return 1;

    cxta_struct_vwap_state_reset(&state);
    for (size_t i = 0; i <= view->index; ++i) {
        (void)cxta_struct_vwap_step(view, i, args, &state, out);
    }
    return 1;
}

void cxta_struct_vwap_state_reset(cxta_struct_vwap_state* state) {
    if (!state) return;
    memset(state, 0, sizeof(*state));
    state->prev_direction = -1.0;
}

int cxta_struct_vwap_step(const cxta_series_bar_view* view,
                          size_t index,
                          const cxta_struct_vwap_args* args,
                          cxta_struct_vwap_state* state,
                          cxta_struct_vwap_result* out) {
    const cxta_series_bar* bar;
    const double atr_period = (double)((args && args->atr_period < 1) ? 1 : args->atr_period);
    const size_t clamped_index =
        (view && view->size > 0u) ? cxta_series_clamp_index(view->size, index) : 0u;
    size_t start;
    double alpha;
    double pxv_parallel;
    double direction;
    double anchor_price;
    size_t anchor_index;
    double p_acc;
    double v_acc;
    double p_bull;
    double v_bull;
    double p_bear;
    double v_bear;

    if (!out) return 0;
    memset(out, 0, sizeof(*out));
    if (!view || !args || !state || !cxta_series_bar_view_valid(view)) return 0;

    bar = cxta_series_bar_view_at(view, clamped_index);
    if (!bar) return 0;

    if (clamped_index == 0u) {
        state->atr = cxta_struct_vwap_true_range_at(view, clamped_index);
        state->atr_avg = state->atr;
    } else {
        const double tr = cxta_struct_vwap_true_range_at(view, clamped_index);
        state->atr = ((state->atr * (atr_period - 1.0)) + tr) / atr_period;
        state->atr_avg = ((state->atr_avg * (atr_period - 1.0)) + state->atr) / atr_period;
    }

    start = (clamped_index + 1u > (size_t)args->swing_period)
                ? (clamped_index + 1u - (size_t)args->swing_period)
                : 0u;
    if (cxta_struct_vwap_is_window_high(view, start, clamped_index)) {
        state->last_high = bar->high;
        state->last_high_index = clamped_index;
        state->last_high_atr = state->atr;
        state->last_high_atr_avg = state->atr_avg;
    }
    if (cxta_struct_vwap_is_window_low(view, start, clamped_index)) {
        state->last_low = bar->low;
        state->last_low_index = clamped_index;
        state->last_low_atr = state->atr;
        state->last_low_atr_avg = state->atr_avg;
    }

    alpha = cxta_struct_vwap_alpha_for_args(args, state->atr, state->atr_avg);
    pxv_parallel = cxta_series_typical_price(bar) * bar->volume;

    p_bull = state->bull_p_acc;
    v_bull = state->bull_v_acc;
    if (cxta_struct_vwap_is_window_low(view, start, clamped_index)) {
        p_bull = bar->low * bar->volume;
        v_bull = bar->volume;
        p_bull = (1.0 - alpha) * p_bull + alpha * pxv_parallel;
        v_bull = (1.0 - alpha) * v_bull + alpha * bar->volume;
        state->bull_have_anchor = 1;
    } else if (state->bull_have_anchor) {
        p_bull = (1.0 - alpha) * p_bull + alpha * pxv_parallel;
        v_bull = (1.0 - alpha) * v_bull + alpha * bar->volume;
    }
    state->bull_p_acc = p_bull;
    state->bull_v_acc = v_bull;

    p_bear = state->bear_p_acc;
    v_bear = state->bear_v_acc;
    if (cxta_struct_vwap_is_window_high(view, start, clamped_index)) {
        p_bear = bar->high * bar->volume;
        v_bear = bar->volume;
        p_bear = (1.0 - alpha) * p_bear + alpha * pxv_parallel;
        v_bear = (1.0 - alpha) * v_bear + alpha * bar->volume;
        state->bear_have_anchor = 1;
    } else if (state->bear_have_anchor) {
        p_bear = (1.0 - alpha) * p_bear + alpha * pxv_parallel;
        v_bear = (1.0 - alpha) * v_bear + alpha * bar->volume;
    }
    state->bear_p_acc = p_bear;
    state->bear_v_acc = v_bear;

    direction = (state->last_high_index > state->last_low_index) ? 1.0 : -1.0;
    anchor_index = state->anchor_index;
    anchor_price = state->anchor_price;
    p_acc = state->p_acc;
    v_acc = state->v_acc;

    if (!state->have_anchor) {
        anchor_index = (direction > 0.0) ? state->last_low_index : state->last_high_index;
        anchor_price = (direction > 0.0) ? state->last_low : state->last_high;
    }

    if (clamped_index > 0u && direction != state->prev_direction) {
        out->reanchor = 1.0;
        anchor_index = (direction > 0.0) ? state->last_low_index : state->last_high_index;
        anchor_price = (direction > 0.0) ? state->last_low : state->last_high;

        if (state->have_prev_ref) {
            if (direction > 0.0) {
                if (state->last_low < state->prev_ref) out->ll = 1.0;
                else if (state->last_low > state->prev_ref) out->hl = 1.0;
            } else {
                if (state->last_high < state->prev_ref) out->lh = 1.0;
                else if (state->last_high > state->prev_ref) out->hh = 1.0;
            }
        }

        state->prev_ref = (direction > 0.0) ? state->prev_high_state : state->prev_low_state;
        state->have_prev_ref = 1;
        cxta_struct_vwap_replay_from_anchor(view,
                                            args,
                                            anchor_index,
                                            clamped_index,
                                            anchor_price,
                                            (direction > 0.0) ? state->last_low_atr
                                                              : state->last_high_atr,
                                            (direction > 0.0) ? state->last_low_atr_avg
                                                              : state->last_high_atr_avg,
                                            &p_acc,
                                            &v_acc);
        state->have_anchor = 1;
    } else {
        const double pxv = cxta_series_typical_price(bar) * bar->volume;
        if (clamped_index == 0u) {
            p_acc = pxv;
            v_acc = bar->volume;
        }
        p_acc = (1.0 - alpha) * p_acc + alpha * pxv;
        v_acc = (1.0 - alpha) * v_acc + alpha * bar->volume;
    }

    state->prev_direction = direction;
    state->prev_high_state = state->last_high;
    state->prev_low_state = state->last_low;
    state->anchor_index = anchor_index;
    state->anchor_price = anchor_price;
    state->p_acc = p_acc;
    state->v_acc = v_acc;

    out->value = (fabs(v_acc) > 1e-12) ? (p_acc / v_acc) : 0.0;
    out->direction = direction;
    out->anchor_price = anchor_price;
    out->anchor_bars = (double)(clamped_index - anchor_index);
    out->last_high = state->last_high;
    out->last_low = state->last_low;
    out->bull_vwap =
        (state->bull_have_anchor && fabs(v_bull) > 1e-12) ? p_bull / v_bull : 0.0;
    out->bear_vwap =
        (state->bear_have_anchor && fabs(v_bear) > 1e-12) ? p_bear / v_bear : 0.0;
    return 1;
}
