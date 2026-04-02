/**
 * @file macd.h
 * @brief Moving Average Convergence Divergence helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MACD output payload.
 */
typedef struct {
    double line;
    double signal;
    double histogram;
} cxta_macd_output;

/**
 * @brief MACD state for incremental updates.
 */
typedef struct {
    double ema_fast;
    double ema_slow;
    double signal;
} cxta_macd_state;

/**
 * @brief Perform one MACD update.
 * @param[in] close Current close.
 * @param[in] fast Fast EMA period.
 * @param[in] slow Slow EMA period.
 * @param[in] signal Signal EMA period.
 * @param[in,out] st MACD state.
 * @return MACD outputs.
 */
cxta_macd_output cxta_macd_step(double close,
                                int fast,
                                int slow,
                                int signal,
                                cxta_macd_state* st);

/**
 * @brief Compute MACD at the current view index.
 * @param[in] view Bar series view.
 * @param[in] fast Fast EMA period.
 * @param[in] slow Slow EMA period.
 * @param[in] signal Signal EMA period.
 * @return MACD outputs.
 */
cxta_macd_output cxta_macd(const cxta_series_bar_view* view,
                           int fast,
                           int slow,
                           int signal);

#ifdef __cplusplus
}
#endif
