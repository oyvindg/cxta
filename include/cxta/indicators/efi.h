/**
 * @file efi.h
 * @brief Elder Force Index helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for Elder Force Index (`elder_force`). */
static const cxta_param_descriptor cxta_elder_force_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for Elder Force Index. */
static const cxta_bridge_fn_spec cxta_elder_force_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("elder_force", 0u, 1u, cxta_elder_force_params, 1);

/** @brief Expression-facing descriptor for Elder Force Index. */
extern const cxta_indicator_descriptor cxta_elder_force_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Elder Force Index state for incremental updates.
 */
typedef struct {
    double value;        /**< Current EFI or smoothed EFI. */
    double prev_close;   /**< Previous close. */
    double initialized;  /**< Non-zero once the first close is seen. */
    double seeded;       /**< Non-zero once the force series is seeded. */
} cxta_efi_state;

/**
 * @brief Perform one EFI update.
 * @param[in] close Current close.
 * @param[in] volume Current volume.
 * @param[in] period EMA smoothing period (`1` for raw EFI).
 * @param[in,out] st EFI state.
 * @return Updated EFI value.
 */
double cxta_efi_step(double close, double volume, int period, cxta_efi_state* st);

/**
 * @brief Compute EFI at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period EMA smoothing period (`1` for raw EFI).
 * @return EFI value, or 0.0 when invalid.
 */
double cxta_efi(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
