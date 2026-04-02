/**
 * @file candles.c
 * @brief Scalar candlestick predicates.
 */

#include <cxta/ts/candles.h>

#include <stddef.h>

static double cxta_ts_abs(double x) {
    return (x < 0.0) ? -x : x;
}

static double cxta_ts_max2(double a, double b) {
    return (a > b) ? a : b;
}

static double cxta_ts_min2(double a, double b) {
    return (a < b) ? a : b;
}

static double cxta_ts_nonneg(double x) {
    return (x < 0.0) ? 0.0 : x;
}

typedef struct {
    double open;
    double high;
    double low;
    double close;
} cxta_ts_candle_geometry;

static double cxta_ts_candle_body(const cxta_ts_candle_geometry* candle) {
    return candle ? cxta_ts_abs(candle->close - candle->open) : 0.0;
}

static double cxta_ts_candle_range(const cxta_ts_candle_geometry* candle) {
    return candle ? (candle->high - candle->low) : 0.0;
}

static double cxta_ts_candle_upper_shadow(const cxta_ts_candle_geometry* candle) {
    if (!candle) return 0.0;
    return candle->high - cxta_ts_max2(candle->open, candle->close);
}

double cxta_ts_candle_bullish_engulfing(double open,
                                        double close,
                                        double prev_open,
                                        double prev_close) {
    const int prev_bearish = prev_close < prev_open;
    const int curr_bullish = close > open;
    const int engulf = (open <= prev_close) && (close >= prev_open);
    return (prev_bearish && curr_bullish && engulf) ? 1.0 : 0.0;
}

double cxta_ts_candle_bearish_engulfing(double open,
                                        double close,
                                        double prev_open,
                                        double prev_close) {
    const int prev_bullish = prev_close > prev_open;
    const int curr_bearish = close < open;
    const int engulf = (open >= prev_close) && (close <= prev_open);
    return (prev_bullish && curr_bearish && engulf) ? 1.0 : 0.0;
}

double cxta_ts_candle_hammer(double open, double high, double low, double close) {
    const double range = high - low;
    const double body = cxta_ts_abs(close - open);
    const double lower_shadow = cxta_ts_min2(open, close) - low;
    const double upper_shadow = high - cxta_ts_max2(open, close);
    const int small_body = range > 0.0 && (body / range) <= 0.35;
    const int long_lower_shadow = lower_shadow >= (body * 2.0);
    const int short_upper_shadow = upper_shadow <= body;
    if (range <= 0.0) return 0.0;
    return (small_body && long_lower_shadow && short_upper_shadow) ? 1.0 : 0.0;
}

double cxta_ts_candle_shooting_star(double open, double high, double low, double close) {
    const double range = high - low;
    const double body = cxta_ts_abs(close - open);
    const double upper_shadow = high - cxta_ts_max2(open, close);
    const double lower_shadow = cxta_ts_min2(open, close) - low;
    const int small_body = range > 0.0 && (body / range) <= 0.35;
    const int long_upper_shadow = upper_shadow >= (body * 2.0);
    const int short_lower_shadow = lower_shadow <= body;
    if (range <= 0.0) return 0.0;
    return (small_body && long_upper_shadow && short_lower_shadow) ? 1.0 : 0.0;
}

double cxta_ts_candle_doji(double open,
                           double high,
                           double low,
                           double close,
                           double threshold) {
    const double range = high - low;
    const double body = cxta_ts_abs(close - open);
    const double max_ratio = cxta_ts_nonneg(threshold);
    if (range <= 0.0) return 0.0;
    return (body <= (range * max_ratio)) ? 1.0 : 0.0;
}

double cxta_ts_candle_middle_finger(const double* args, size_t nargs) {
    size_t rem;
    size_t n_candle_args;
    size_t n_candles;
    size_t mid_idx;
    size_t mid_base;
    cxta_ts_candle_geometry middle;
    double middle_range;
    double shoulder_tol = 0.07;
    double head_ratio = 1.04;
    double shoulder_max_high = 0.0;
    double shoulder_min_high = 0.0;
    double shoulder_body_ceiling = 0.0;
    size_t i;

    if (!args || nargs < 12u) return 0.0;

    rem = nargs % 4u;
    if (rem == 3u) return 0.0;

    n_candle_args = nargs;
    if (rem == 2u) {
        shoulder_tol = cxta_ts_nonneg(args[nargs - 2u]);
        head_ratio = cxta_ts_max2(1.0, args[nargs - 1u]);
        n_candle_args = nargs - 2u;
    } else if (rem == 1u) {
        shoulder_tol = cxta_ts_nonneg(args[nargs - 1u]);
        n_candle_args = nargs - 1u;
    }

    if (n_candle_args < 12u) return 0.0;

    n_candles = n_candle_args / 4u;
    if (n_candles < 3u || (n_candles % 2u) == 0u) return 0.0;

    mid_idx = n_candles / 2u;
    mid_base = mid_idx * 4u;
    middle.open = args[mid_base];
    middle.high = args[mid_base + 1u];
    middle.low = args[mid_base + 2u];
    middle.close = args[mid_base + 3u];
    middle_range = cxta_ts_candle_range(&middle);
    if (middle_range <= 0.0) return 0.0;

    shoulder_min_high = middle.high;
    for (i = 0u; i < n_candles; ++i) {
        cxta_ts_candle_geometry candle;
        size_t base;

        if (i == mid_idx) continue;
        base = i * 4u;
        candle.open = args[base];
        candle.high = args[base + 1u];
        candle.low = args[base + 2u];
        candle.close = args[base + 3u];
        if (cxta_ts_candle_range(&candle) <= 0.0) return 0.0;

        shoulder_max_high = cxta_ts_max2(shoulder_max_high, candle.high);
        shoulder_min_high = cxta_ts_min2(shoulder_min_high, candle.high);
        shoulder_body_ceiling = cxta_ts_max2(
            shoulder_body_ceiling,
            cxta_ts_max2(candle.open, candle.close));
    }

    if (shoulder_max_high <= 0.0) return 0.0;

    {
        const int shoulders_aligned =
            (shoulder_max_high - shoulder_min_high) <= (shoulder_max_high * shoulder_tol);
        const int dominant_middle_high =
            middle.high >= shoulder_max_high * head_ratio &&
            middle.high > shoulder_max_high;
        const double middle_body = cxta_ts_candle_body(&middle);
        const int middle_long_upper_wick =
            cxta_ts_candle_upper_shadow(&middle) >=
            cxta_ts_max2(middle_body * 1.5, middle_range * 0.30);
        const size_t right_base = (n_candles - 1u) * 4u;
        const cxta_ts_candle_geometry rightmost = {
            args[right_base],
            args[right_base + 1u],
            args[right_base + 2u],
            args[right_base + 3u],
        };
        const int right_bearish = rightmost.close < rightmost.open;
        const int right_confirms_rejection = rightmost.close <= shoulder_body_ceiling;

        return (shoulders_aligned &&
                dominant_middle_high &&
                middle_long_upper_wick &&
                right_bearish &&
                right_confirms_rejection)
                   ? 1.0
                   : 0.0;
    }
}
