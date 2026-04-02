/**
 * @file ppo.h
 * @brief Percentage Price Oscillator helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief PPO state for incremental updates.
 */
typedef struct {
    double ema_fast;    /**< Fast EMA state. */
    double ema_slow;    /**< Slow EMA state. */
    double initialized; /**< Non-zero once seeded. */
} cxta_ppo_state;

/**
 * @brief Perform one PPO update.
 * @param[in] close Current close.
 * @param[in] fast Fast EMA period.
 * @param[in] slow Slow EMA period.
 * @param[in,out] st PPO state.
 * @return Updated PPO value.
 */
double cxta_ppo_step(double close, int fast, int slow, cxta_ppo_state* st);

/**
 * @brief Compute PPO at the current view index.
 * @param[in] view Bar series view.
 * @param[in] fast Fast EMA period.
 * @param[in] slow Slow EMA period.
 * @return PPO value, or 0.0 when invalid.
 */
double cxta_ppo(const cxta_series_bar_view* view, int fast, int slow);

#ifdef __cplusplus
}
#endif
