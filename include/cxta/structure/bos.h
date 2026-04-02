/**
 * @file bos.h
 * @brief Break-of-structure and change-of-character helpers.
 *
 * A Break of Structure (BOS) occurs when price closes beyond a confirmed
 * pivot level:
 *   - bos_up   — close exceeds the last confirmed pivot high
 *   - bos_down — close falls below the last confirmed pivot low
 *
 * A Change of Character (CHoCH) is a BOS that contradicts the prevailing
 * two-pivot trend:
 *   - choch_up   — bos_up while structure is bearish (LH + LL)
 *   - choch_down — bos_down while structure is bullish (HH + HL)
 */

#pragma once

#include "pivot.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief BOS/CHoCH snapshot for the current bar.
 */
typedef struct {
    double bos_up;    /**< 1.0 when close broke above the last confirmed pivot high. */
    double bos_down;  /**< 1.0 when close broke below the last confirmed pivot low.  */
    double level;     /**< Pivot level that was broken; 0.0 when no break occurred.  */
    double choch_up;  /**< 1.0 when bos_up occurs within a LH+LL downtrend context. */
    double choch_down;/**< 1.0 when bos_down occurs within a HH+HL uptrend context. */
} cxta_struct_bos_state;

/**
 * @brief Compute BOS/CHoCH state from the current bar and pivot context.
 *
 * The prevailing trend is determined from the last two confirmed pivot highs
 * and lows held in @p pivot.  A full HH+HL combination is treated as a
 * bullish trend; a full LH+LL combination is treated as a bearish trend.
 * Mixed or incomplete pivot history produces no CHoCH signal.
 *
 * @param[in]  view   Current bar series view (close of the current bar is used).
 * @param[in]  pivot  Pivot state providing confirmed high/low levels and history.
 * @param[out] out    Populated BOS/CHoCH snapshot.
 * @return 1 on success, 0 on NULL argument.
 */
int cxta_struct_bos_state_compute(const cxta_series_bar_view*   view,
                           const cxta_struct_pivot_state* pivot,
                           cxta_struct_bos_state*         out);

#ifdef __cplusplus
}
#endif
