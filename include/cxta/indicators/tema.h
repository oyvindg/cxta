/**
 * @file tema.h
 * @brief Triple Exponential Moving Average helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_tema_params[] = {
    {"period"},
};
static const cxta_bridge_fn_spec cxta_tema_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("tema", 1u, 1u, cxta_tema_params, 1);

extern const cxta_indicator_descriptor cxta_tema_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TEMA state for incremental updates.
 */
typedef struct {
    double ema1;        /**< First EMA value. */
    double ema2;        /**< EMA of EMA value. */
    double ema3;        /**< EMA of EMA of EMA value. */
    double initialized; /**< Non-zero once seeded. */
} cxta_tema_state;

/**
 * @brief Perform one TEMA update.
 * @param[in]     close  Current close.
 * @param[in]     period EMA period.
 * @param[in,out] st     TEMA state.
 * @return TEMA value (3*EMA - 3*EMA(EMA) + EMA(EMA(EMA))).
 */
double cxta_tema_step(double close, int period, cxta_tema_state* st);

/**
 * @brief Compute close-based TEMA at the current view index.
 * @param[in] view   Bar series view.
 * @param[in] period EMA period.
 * @return TEMA value, or 0.0 when the view is invalid.
 */
double cxta_tema(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
