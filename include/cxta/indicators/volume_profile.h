/**
 * @file volume_profile.h
 * @brief Rolling volume-profile helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_volume_profile_params[] = {
    {"period"},
    {"bins"},
};

static const cxta_bridge_fn_spec cxta_volume_profile_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("volume_profile", 1u, 2u, cxta_volume_profile_params, 1);

extern const cxta_indicator_descriptor cxta_volume_profile_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Volume-profile output payload.
 */
typedef struct {
    double poc;
    double vah;
    double val;
    double range_high;
    double range_low;
} cxta_vp_output;

/**
 * @brief Compute rolling volume-profile outputs at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @param[in] bins Number of profile buckets.
 * @return Volume-profile outputs, or a zero-initialized struct when invalid.
 */
cxta_vp_output cxta_volume_profile(const cxta_series_bar_view* view,
                                   int period,
                                   int bins);

#ifdef __cplusplus
}
#endif
