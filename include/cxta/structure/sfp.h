/**
 * @file sfp.h
 * @brief Swing Failure Pattern (SFP) helpers.
 */

#pragma once

#include "pivot.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Swing-failure snapshot for the current bar.
 */
typedef struct {
    double sfp_high; /**< 1.0 when price wicks above pivot high and closes back below. */
    double sfp_low;  /**< 1.0 when price wicks below pivot low and closes back above.   */
    double level;    /**< Pivot level involved in the swing-failure event.              */
} cxta_struct_sfp_state;

/**
 * @brief Compute SFP state from current bar and pivot context.
 *
 * High-side SFP:
 * - current high > last confirmed pivot high
 * - current close < last confirmed pivot high
 *
 * Low-side SFP:
 * - current low < last confirmed pivot low
 * - current close > last confirmed pivot low
 *
 * @param[in]  view   Current bar series view.
 * @param[in]  pivot  Pivot state with latest confirmed high/low.
 * @param[out] out    Populated SFP snapshot.
 * @return 1 on success, 0 on NULL argument.
 */
int cxta_struct_sfp_state_compute(const cxta_series_bar_view*   view,
                          const cxta_struct_pivot_state* pivot,
                          cxta_struct_sfp_state*         out);

#ifdef __cplusplus
}
#endif
