/**
 * @file candles.h
 * @brief Scalar candlestick predicates.
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Detect a bullish engulfing candle pair.
 * @param[in] open Current candle open.
 * @param[in] close Current candle close.
 * @param[in] prev_open Previous candle open.
 * @param[in] prev_close Previous candle close.
 * @return 1.0 when a bullish engulfing pair is detected, otherwise 0.0.
 */
double cxta_ts_candle_bullish_engulfing(double open,
                                        double close,
                                        double prev_open,
                                        double prev_close);

/**
 * @brief Detect a bearish engulfing candle pair.
 * @param[in] open Current candle open.
 * @param[in] close Current candle close.
 * @param[in] prev_open Previous candle open.
 * @param[in] prev_close Previous candle close.
 * @return 1.0 when a bearish engulfing pair is detected, otherwise 0.0.
 */
double cxta_ts_candle_bearish_engulfing(double open,
                                        double close,
                                        double prev_open,
                                        double prev_close);

/**
 * @brief Detect a hammer candlestick.
 * @param[in] open Candle open.
 * @param[in] high Candle high.
 * @param[in] low Candle low.
 * @param[in] close Candle close.
 * @return 1.0 when a hammer is detected, otherwise 0.0.
 */
double cxta_ts_candle_hammer(double open, double high, double low, double close);

/**
 * @brief Detect a shooting-star candlestick.
 * @param[in] open Candle open.
 * @param[in] high Candle high.
 * @param[in] low Candle low.
 * @param[in] close Candle close.
 * @return 1.0 when a shooting star is detected, otherwise 0.0.
 */
double cxta_ts_candle_shooting_star(double open, double high, double low, double close);

/**
 * @brief Detect a doji candlestick.
 * @param[in] open Candle open.
 * @param[in] high Candle high.
 * @param[in] low Candle low.
 * @param[in] close Candle close.
 * @param[in] threshold Maximum body/range ratio to accept as a doji.
 * @return 1.0 when a doji is detected, otherwise 0.0.
 */
double cxta_ts_candle_doji(double open,
                           double high,
                           double low,
                           double close,
                           double threshold);

/**
 * @brief Detect an odd-N bearish rejection pattern with a dominant middle wick/high.
 * @param[in] args N×4 OHLC values ordered oldest->newest, plus 0-2 optional params.
 * @param[in] nargs Number of arguments in `args`.
 * @return 1.0 when the pattern is detected, otherwise 0.0.
 */
double cxta_ts_candle_middle_finger(const double* args, size_t nargs);

#ifdef __cplusplus
}
#endif
