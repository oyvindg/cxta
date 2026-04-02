/**
 * @file fvg.h
 * @brief Fair Value Gap (FVG) helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fair Value Gap snapshot for current context.
 */
typedef struct {
    double bullish; /**< 1.0 when latest detected FVG is bullish. */
    double bearish; /**< 1.0 when latest detected FVG is bearish. */
    double top;     /**< Upper boundary of the latest FVG zone.    */
    double bottom;  /**< Lower boundary of the latest FVG zone.    */
    double filled;  /**< 1.0 when the latest FVG has been filled.  */
} cxta_struct_fvg_state;

/**
 * @brief Compute latest FVG state inside a lookback window.
 *
 * A bullish FVG is detected when `high[i-2] < low[i]`.
 * A bearish FVG is detected when `low[i-2] > high[i]`.
 *
 * @param[in]  view      Current bar view.
 * @param[in]  lookback  Number of bars to scan (0 means full history to index).
 * @param[out] out       Populated FVG state.
 * @return 1 on success, 0 on NULL argument.
 */
int cxta_struct_fvg_state_compute(const cxta_series_bar_view* view,
                          size_t              lookback,
                          cxta_struct_fvg_state*      out);

#ifdef __cplusplus
}
#endif
