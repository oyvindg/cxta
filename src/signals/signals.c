/**
 * @file signals.c
 * @brief Scalar signal helpers for expression runtimes.
 */

#include <cxta/signals/signals.h>

#include <cxta/math/math.h>
#include <cxta/series/args.h>
#include <cxta/ts/candles.h>
#include <cxta/ts/cross.h>
#include <cxta/ts/fib.h>
#include <cxta/ts/patterns.h>

typedef struct cxta_signal_pair_args {
    double left;
    double right;
} cxta_signal_pair_args;

typedef struct cxta_signal_cross_args {
    double cur_left;
    double cur_right;
    double prev_left;
    double prev_right;
} cxta_signal_cross_args;

static int cxta_signal_pair_args_parse(const double* args,
                                       size_t nargs,
                                       cxta_signal_pair_args* out) {
    if (!args || nargs < 2 || !out) return 0;
    out->left = args[0];
    out->right = args[1];
    return 1;
}

static int cxta_signal_cross_args_parse(const double* args,
                                        size_t nargs,
                                        cxta_signal_cross_args* out) {
    if (!args || nargs < 4 || !out) return 0;
    out->cur_left = args[0];
    out->cur_right = args[1];
    out->prev_left = args[2];
    out->prev_right = args[3];
    return 1;
}

static double cxta_signal_max(double a, double b) {
    return (a > b) ? a : b;
}

double cxta_signal_above(const double* args, size_t nargs) {
    cxta_signal_pair_args in;
    if (!cxta_signal_pair_args_parse(args, nargs, &in)) return 0.0;
    return (cxta_math_cmp(in.left, in.right) > 0.0) ? 1.0 : 0.0;
}

double cxta_signal_below(const double* args, size_t nargs) {
    cxta_signal_pair_args in;
    if (!cxta_signal_pair_args_parse(args, nargs, &in)) return 0.0;
    return (cxta_math_cmp(in.left, in.right) < 0.0) ? 1.0 : 0.0;
}

double cxta_signal_score(const double* args, size_t nargs) {
    if (!args || nargs < 3) return 0.0;

    const double value = args[0];
    const double from = args[1];
    const double to = args[2];
    if (cxta_math_cmp(from, to) == 0.0) return 0.0;

    if (from < to) {
        if (value <= from) return 0.0;
        if (value >= to) return 1.0;
        return (value - from) / (to - from);
    }

    if (value >= from) return 0.0;
    if (value <= to) return 1.0;
    return (from - value) / (from - to);
}

double cxta_signal_cross(const double* args, size_t nargs) {
    if (!args || nargs < 2) return 0.0;
    return (args[0] > args[1]) ? 1.0 : 0.0;
}

double cxta_signal_cross_above(const double* args, size_t nargs) {
    if (args && nargs == 2) {
        static _Thread_local int has_prev = 0;
        static _Thread_local double prev_left = 0.0;
        static _Thread_local double prev_right = 0.0;

        const double cur_left = args[0];
        const double cur_right = args[1];
        if (!has_prev) {
            prev_left = cur_left;
            prev_right = cur_right;
            has_prev = 1;
            return 0.0;
        }

        const double out = cxta_ts_crossed_above(prev_left, prev_right, cur_left, cur_right);
        prev_left = cur_left;
        prev_right = cur_right;
        return out;
    }

    cxta_signal_cross_args in;
    if (!cxta_signal_cross_args_parse(args, nargs, &in)) return 0.0;
    return cxta_ts_crossed_above(in.prev_left, in.prev_right, in.cur_left, in.cur_right);
}

double cxta_signal_cross_below(const double* args, size_t nargs) {
    if (args && nargs == 2) {
        static _Thread_local int has_prev = 0;
        static _Thread_local double prev_left = 0.0;
        static _Thread_local double prev_right = 0.0;

        const double cur_left = args[0];
        const double cur_right = args[1];
        if (!has_prev) {
            prev_left = cur_left;
            prev_right = cur_right;
            has_prev = 1;
            return 0.0;
        }

        const double out = cxta_ts_crossed_below(prev_left, prev_right, cur_left, cur_right);
        prev_left = cur_left;
        prev_right = cur_right;
        return out;
    }

    cxta_signal_cross_args in;
    if (!cxta_signal_cross_args_parse(args, nargs, &in)) return 0.0;
    return cxta_ts_crossed_below(in.prev_left, in.prev_right, in.cur_left, in.cur_right);
}

static double cxta_signal_bullish_engulfing(const double* args, size_t nargs) {
    cxta_series_oc_pair_args in;
    if (!cxta_series_oc_pair_args_parse(args, nargs, &in)) return 0.0;
    return cxta_ts_candle_bullish_engulfing(
        in.open, in.close, in.prev_open, in.prev_close);
}

static double cxta_signal_bearish_engulfing(const double* args, size_t nargs) {
    cxta_series_oc_pair_args in;
    if (!cxta_series_oc_pair_args_parse(args, nargs, &in)) return 0.0;
    return cxta_ts_candle_bearish_engulfing(
        in.open, in.close, in.prev_open, in.prev_close);
}

static double cxta_signal_hammer(const double* args, size_t nargs) {
    cxta_series_bar_args in;
    if (!cxta_series_bar_args_parse(args, nargs, &in)) return 0.0;
    return cxta_ts_candle_hammer(in.open, in.high, in.low, in.close);
}

static double cxta_signal_shooting_star(const double* args, size_t nargs) {
    cxta_series_bar_args in;
    if (!cxta_series_bar_args_parse(args, nargs, &in)) return 0.0;
    return cxta_ts_candle_shooting_star(in.open, in.high, in.low, in.close);
}

static double cxta_signal_doji(const double* args, size_t nargs) {
    cxta_series_bar_args in;
    if (!cxta_series_bar_args_parse(args, nargs, &in)) return 0.0;
    return cxta_ts_candle_doji(
        in.open, in.high, in.low, in.close, (nargs >= 5) ? cxta_signal_max(0.0, args[4]) : 0.10);
}

static double cxta_signal_double_top(const double* args, size_t nargs) {
    if (!args || nargs < 4) return 0.0;
    const double peak_tol = (nargs >= 5) ? cxta_signal_max(0.0, args[4]) : 0.03;
    return cxta_ts_pattern_double_top(args[0], args[1], args[2], args[3], peak_tol);
}

static double cxta_signal_double_bottom(const double* args, size_t nargs) {
    if (!args || nargs < 4) return 0.0;
    const double trough_tol = (nargs >= 5) ? cxta_signal_max(0.0, args[4]) : 0.03;
    return cxta_ts_pattern_double_bottom(args[0], args[1], args[2], args[3], trough_tol);
}

static double cxta_signal_head_and_shoulders(const double* args, size_t nargs) {
    if (!args || nargs < 5) return 0.0;
    const double shoulder_tol = (nargs >= 6) ? cxta_signal_max(0.0, args[5]) : 0.05;
    return cxta_ts_pattern_head_and_shoulders(
        args[0], args[1], args[2], args[3], args[4], shoulder_tol);
}

static double cxta_signal_inverse_head_and_shoulders(const double* args, size_t nargs) {
    if (!args || nargs < 5) return 0.0;
    const double shoulder_tol = (nargs >= 6) ? cxta_signal_max(0.0, args[5]) : 0.05;
    return cxta_ts_pattern_inverse_head_and_shoulders(
        args[0], args[1], args[2], args[3], args[4], shoulder_tol);
}

static double cxta_signal_cup_and_handle(const double* args, size_t nargs) {
    if (!args || nargs < 5) return 0.0;
    const double symmetry_tol = (nargs >= 6) ? cxta_signal_max(0.0, args[5]) : 0.06;
    const double handle_max_ratio = (nargs >= 7) ? cxta_signal_max(0.05, args[6]) : 0.5;
    return cxta_ts_pattern_cup_and_handle(
        args[0], args[1], args[2], args[3], args[4], symmetry_tol, handle_max_ratio);
}

static double cxta_signal_inverted_cup_and_handle(const double* args, size_t nargs) {
    if (!args || nargs < 5) return 0.0;
    const double symmetry_tol = (nargs >= 6) ? cxta_signal_max(0.0, args[5]) : 0.06;
    const double handle_max_ratio = (nargs >= 7) ? cxta_signal_max(0.05, args[6]) : 0.5;
    return cxta_ts_pattern_inverted_cup_and_handle(
        args[0], args[1], args[2], args[3], args[4], symmetry_tol, handle_max_ratio);
}

static double cxta_signal_fib_level(const double* args, size_t nargs) {
    if (!args || nargs < 3) return 0.0;
    return cxta_ts_fib_level(args[0], args[1], args[2]);
}

static double cxta_signal_fib_near(const double* args, size_t nargs) {
    if (!args || nargs < 4) return 0.0;
    const double tolerance = (nargs >= 5) ? args[4] : 0.02;
    return cxta_ts_fib_near(args[0], args[1], args[2], args[3], tolerance);
}

static double cxta_signal_fib_zone(const double* args, size_t nargs) {
    if (!args || nargs < 5) return 0.0;
    return cxta_ts_fib_zone(args[0], args[1], args[2], args[3], args[4]);
}

static double cxta_signal_fib_extension(const double* args, size_t nargs) {
    if (!args || nargs < 4) return 0.0;
    return cxta_ts_fib_extension(args[0], args[1], args[2], args[3]);
}

static double cxta_signal_stacked_descending(const double* args, size_t nargs) {
    if (!args || nargs < 2) return 0.0;
    for (size_t i = 0; i + 1 < nargs; ++i) {
        if (!(args[i] > args[i + 1])) return 0.0;
    }
    return 1.0;
}

static const cxta_signal_pattern_descriptor cxta_signal_pattern_table[] = {
    {"above",                      cxta_signal_above,                       2,  2, CXTA_SIGNAL_PATTERN_BOOL   },
    {"below",                      cxta_signal_below,                       2,  2, CXTA_SIGNAL_PATTERN_BOOL   },
    {"score",                      cxta_signal_score,                       3,  3, CXTA_SIGNAL_PATTERN_NUMBER },
    {"cross",                      cxta_signal_cross,                       2,  2, CXTA_SIGNAL_PATTERN_BOOL   },
    {"cross_above",                cxta_signal_cross_above,                 2,  4, CXTA_SIGNAL_PATTERN_BOOL   },
    {"cross_below",                cxta_signal_cross_below,                 2,  4, CXTA_SIGNAL_PATTERN_BOOL   },
    {"bullish_engulfing",          cxta_signal_bullish_engulfing,           4,  4, CXTA_SIGNAL_PATTERN_BOOL   },
    {"bearish_engulfing",          cxta_signal_bearish_engulfing,           4,  4, CXTA_SIGNAL_PATTERN_BOOL   },
    {"hammer",                     cxta_signal_hammer,                      4,  4, CXTA_SIGNAL_PATTERN_BOOL   },
    {"shooting_star",              cxta_signal_shooting_star,               4,  4, CXTA_SIGNAL_PATTERN_BOOL   },
    {"doji",                       cxta_signal_doji,                        4,  5, CXTA_SIGNAL_PATTERN_BOOL   },
    {"middle_finger",              cxta_ts_candle_middle_finger,           12, 50, CXTA_SIGNAL_PATTERN_BOOL   },
    {"double_top",                 cxta_signal_double_top,                  4,  5, CXTA_SIGNAL_PATTERN_BOOL   },
    {"double_bottom",              cxta_signal_double_bottom,               4,  5, CXTA_SIGNAL_PATTERN_BOOL   },
    {"head_and_shoulders",         cxta_signal_head_and_shoulders,          5,  6, CXTA_SIGNAL_PATTERN_BOOL   },
    {"inverse_head_and_shoulders", cxta_signal_inverse_head_and_shoulders,  5,  6, CXTA_SIGNAL_PATTERN_BOOL   },
    {"cup_and_handle",             cxta_signal_cup_and_handle,              5,  7, CXTA_SIGNAL_PATTERN_BOOL   },
    {"inverted_cup_and_handle",    cxta_signal_inverted_cup_and_handle,     5,  7, CXTA_SIGNAL_PATTERN_BOOL   },
    {"stacked",                    cxta_signal_stacked_descending,          2, 16, CXTA_SIGNAL_PATTERN_BOOL   },
    {"fib_level",                  cxta_signal_fib_level,                   3,  3, CXTA_SIGNAL_PATTERN_NUMBER },
    {"fib_near",                   cxta_signal_fib_near,                    4,  5, CXTA_SIGNAL_PATTERN_BOOL   },
    {"fib_zone",                   cxta_signal_fib_zone,                    5,  5, CXTA_SIGNAL_PATTERN_BOOL   },
    {"fib_extension",              cxta_signal_fib_extension,               4,  4, CXTA_SIGNAL_PATTERN_NUMBER },
};

const cxta_signal_pattern_descriptor* cxta_signal_pattern_descriptors(size_t* out_count) {
    if (out_count) {
        *out_count = sizeof(cxta_signal_pattern_table) / sizeof(cxta_signal_pattern_table[0]);
    }
    return cxta_signal_pattern_table;
}
