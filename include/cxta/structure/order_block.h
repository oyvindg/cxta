/**
 * @file order_block.h
 * @brief Order Block helpers built on top of BOS and pivots.
 */

#pragma once

#include "bos.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Order Block snapshot for the current bar.
 */
typedef struct {
    double bullish;  /**< 1.0 for bullish order block, 0.0 otherwise. */
    double bearish;  /**< 1.0 for bearish order block, 0.0 otherwise. */
    double top;      /**< Upper boundary of block (candle body).      */
    double bottom;   /**< Lower boundary of block (candle body).      */
    double strength; /**< Normalized displacement strength [0,1].      */
} cxta_struct_order_block_state;

/**
 * @brief Compute the latest BOS-derived order block in the lookback window.
 *
 * The latest BOS event is searched backwards from the current index. For:
 * - BOS up: pick the nearest bearish candle before the BOS bar.
 * - BOS down: pick the nearest bullish candle before the BOS bar.
 *
 * @param[in]  view   Current bar series view.
 * @param[in]  args   Parsed pivot args used to derive BOS history.
 * @param[out] out    Populated order block snapshot.
 * @return 1 on success, 0 on NULL argument.
 */
int cxta_struct_order_block_state_compute(const cxta_series_bar_view*  view,
                                  const cxta_struct_pivot_args* args,
                                  cxta_struct_order_block_state* out);

#ifdef __cplusplus
}
#endif
