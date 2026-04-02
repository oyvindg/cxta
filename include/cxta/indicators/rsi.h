/**
 * @file rsi.h
 * @brief Relative Strength Index (RSI).
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief RSI state for incremental updates.
 */
typedef struct {
    double avg_gain;
    double avg_loss;
} cxta_rsi_state;

/**
 * @brief Seed RSI state from the first @p period close deltas.
 * @param[in] view Bar series view.
 * @param[in] period RSI period.
 * @param[out] st State to initialize.
 */
void cxta_rsi_seed(const cxta_series_bar_view* view, int period, cxta_rsi_state* st);

/**
 * @brief Perform one RSI update.
 * @param[in] close Current close.
 * @param[in] prev_close Previous close.
 * @param[in] period RSI period.
 * @param[in,out] st Seeded RSI state.
 * @return RSI value in `[0, 100]`.
 */
double cxta_rsi_step(double close, double prev_close, int period, cxta_rsi_state* st);

/**
 * @brief Compute RSI at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period RSI period.
 * @return RSI value in `[0, 100]`.
 */
double cxta_rsi(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
