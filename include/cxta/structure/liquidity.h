/**
 * @file liquidity.h
 * @brief Liquidity zone helpers derived from pivot state.
 */

#pragma once

#include "pivot.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Liquidity zone snapshot for the current bar.
 */
typedef struct {
    double buy_side;  /**< 1.0 when equal highs imply buy-side liquidity.   */
    double sell_side; /**< 1.0 when equal lows imply sell-side liquidity.   */
    double level;     /**< Liquidity level for the active side.             */
    double swept;     /**< 1.0 when current bar sweeps the active level.    */
} cxta_struct_liquidity_state;

/**
 * @brief Compute liquidity-zone state from pivot context and current bar.
 *
 * Equal highs/lows are detected from the latest and previous confirmed pivots.
 * Two highs are considered equal when:
 * `abs(h1-h2)/max(abs(h1), abs(h2), 1e-12) <= tolerance`.
 * The same rule applies for lows.
 *
 * @param[in]  view       Current bar series view.
 * @param[in]  pivot      Pivot state with `high/prev_high/low/prev_low`.
 * @param[in]  tolerance  Relative tolerance for equal-level matching.
 * @param[out] out        Populated liquidity-zone snapshot.
 * @return 1 on success, 0 on NULL argument.
 */
int cxta_struct_liquidity_state_compute(const cxta_series_bar_view*   view,
                                const cxta_struct_pivot_state* pivot,
                                double                tolerance,
                                cxta_struct_liquidity_state*  out);

#ifdef __cplusplus
}
#endif
