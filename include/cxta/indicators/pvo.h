/**
 * @file pvo.h
 * @brief Percentage Volume Oscillator helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief PVO output payload.
 */
typedef struct {
    double line;      /**< Fast/slow percentage spread. */
    double signal;    /**< Signal EMA of the line. */
    double histogram; /**< Line - signal. */
} cxta_pvo_output;

/**
 * @brief PVO state for incremental updates.
 */
typedef struct {
    double ema_fast;    /**< Fast EMA state. */
    double ema_slow;    /**< Slow EMA state. */
    double signal;      /**< Signal EMA state. */
    double initialized; /**< Non-zero once seeded. */
} cxta_pvo_state;

/**
 * @brief Perform one PVO update.
 * @param[in] volume Current volume.
 * @param[in] fast Fast EMA period.
 * @param[in] slow Slow EMA period.
 * @param[in] signal Signal EMA period.
 * @param[in,out] st PVO state.
 * @return Updated PVO outputs.
 */
cxta_pvo_output cxta_pvo_step(double volume,
                              int fast,
                              int slow,
                              int signal,
                              cxta_pvo_state* st);

/**
 * @brief Compute PVO outputs at the current view index.
 * @param[in] view Bar series view.
 * @param[in] fast Fast EMA period.
 * @param[in] slow Slow EMA period.
 * @param[in] signal Signal EMA period.
 * @return PVO outputs, or a zero-initialized struct when invalid.
 */
cxta_pvo_output cxta_pvo(const cxta_series_bar_view* view, int fast, int slow, int signal);

#ifdef __cplusplus
}
#endif
