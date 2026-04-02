/**
 * @file psar.h
 * @brief Parabolic SAR helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Parabolic SAR output payload.
 */
typedef struct {
    double value;
    double direction;
} cxta_psar_output;

/**
 * @brief Incremental Parabolic SAR state.
 *
 * Reserved for future incremental consumers. The standalone replay helper
 * `cxta_psar()` does not require pre-seeded state.
 */
typedef struct {
    double sar;
    double ep;
    double af;
    double direction;
} cxta_psar_state;

/**
 * @brief Compute Parabolic SAR at the current view index.
 * @param[in] view Bar series view.
 * @param[in] step Acceleration step.
 * @param[in] max_step Maximum acceleration step.
 * @return Parabolic SAR outputs, or a zero-initialized struct when invalid.
 */
cxta_psar_output cxta_psar(const cxta_series_bar_view* view,
                           double step,
                           double max_step);

#ifdef __cplusplus
}
#endif
