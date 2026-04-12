/**
 * @file ichimoku.h
 * @brief Ichimoku Cloud helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_ichimoku_params[] = {
    {"tenkan"},
    {"kijun"},
    {"span_b"},
};

static const cxta_bridge_fn_spec cxta_ichimoku_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("ichimoku", 3u, 3u, cxta_ichimoku_params, 1);

extern const cxta_indicator_descriptor cxta_ichimoku_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Ichimoku output payload.
 */
typedef struct {
    double tenkan;   /**< Conversion line. */
    double kijun;    /**< Base line. */
    double senkou_a; /**< Leading span A. */
    double senkou_b; /**< Leading span B. */
    double chikou;   /**< Lagging span value at the current close. */
} cxta_ichimoku_output;

/**
 * @brief Compute Ichimoku outputs at the current view index.
 * @param[in] view Bar series view.
 * @param[in] tenkan_period Tenkan lookback period.
 * @param[in] kijun_period Kijun lookback period.
 * @param[in] senkou_b_period Senkou B lookback period.
 * @return Ichimoku outputs, or a zero-initialized struct when invalid.
 */
cxta_ichimoku_output cxta_ichimoku(const cxta_series_bar_view* view,
                                   int tenkan_period,
                                   int kijun_period,
                                   int senkou_b_period);

#ifdef __cplusplus
}
#endif
