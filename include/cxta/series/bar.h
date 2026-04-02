/**
 * @file bar.h
 * @brief Bar-series views and index helpers.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Clamp an index to valid bounds `[0, size-1]`.
 * @param[in] size Number of elements.
 * @param[in] index Requested index.
 * @return Clamped index (`0` when `size==0`).
 */
size_t cxta_series_clamp_index(size_t size, size_t index);

/**
 * @brief Resolve previous index (`index - 1`) clamped to valid bounds.
 * @param[in] size Number of elements.
 * @param[in] index Current index.
 * @return Previous index, clamped to `[0, size-1]` (`0` when `size==0`).
 */
size_t cxta_series_prev_index(size_t size, size_t index);

/**
 * @brief Resolve lag index (`index - lag`) clamped to valid bounds.
 * @param[in] size Number of elements.
 * @param[in] index Current index.
 * @param[in] lag Lookback bars.
 * @return Lagged index, clamped to `[0, size-1]` (`0` when `size==0`).
 */
size_t cxta_series_lag_index(size_t size, size_t index, size_t lag);

/**
 * @brief Plain OHLCVT bar payload for C code.
 */
typedef struct {
    uint64_t timestamp;
    double open;
    double high;
    double low;
    double close;
    double volume;
} cxta_series_bar;

/**
 * @brief Array-of-structs (AoS) bar view.
 */
typedef struct {
    const cxta_series_bar* bars;
    size_t size;
    size_t index;
} cxta_series_bar_view;

/**
 * @brief Callback that resolves a host-issued handle to a bar-series view.
 *
 * Implemented by the host runtime and passed into cxta timeframe helpers.
 * Returns non-zero on success and fills `out`; returns zero on failure.
 */
typedef int (*cxta_series_resolver_fn)(uint64_t handle,
                                       cxta_series_bar_view* out,
                                       void* userdata);

/**
 * @brief Resolver function pointer bundled with opaque host userdata.
 */
typedef struct {
    cxta_series_resolver_fn resolve;
    void* userdata;
} cxta_series_resolver;

/**
 * @brief Structure-of-arrays (SoA) bar view.
 */
typedef struct {
    const double* open;
    const double* high;
    const double* low;
    const double* close;
    const double* volume;
    size_t size;
    size_t index;
} cxta_series_soa_view;

/**
 * @brief Generic scalar time-series view.
 */
typedef struct {
    const double* values;
    size_t size;
    size_t index;
} cxta_series_scalar_view;

/**
 * @brief Construct a clamped AoS view.
 */
cxta_series_bar_view cxta_series_bar_view_make(const cxta_series_bar* bars, size_t size, size_t index);
/**
 * @brief Validate an AoS view.
 */
int cxta_series_bar_view_valid(const cxta_series_bar_view* view);
/**
 * @brief Read bar at index from AoS view.
 */
const cxta_series_bar* cxta_series_bar_view_at(const cxta_series_bar_view* view, size_t i);
/**
 * @brief Read current bar from AoS view.
 */
const cxta_series_bar* cxta_series_bar_view_current(const cxta_series_bar_view* view);
/**
 * @brief Compute typical price `(high + low + close) / 3` for one bar.
 */
double cxta_series_typical_price(const cxta_series_bar* bar);

/**
 * @brief Find the index of the first bar of the current UTC calendar day.
 *
 * Scans backwards from `idx` through `bars` until the UTC day
 * (`timestamp / 86400`) changes or the beginning of the array is reached.
 *
 * @param[in] bars  Bar array.
 * @param[in] size  Number of bars in the array.
 * @param[in] idx   Current bar index (clamped to `[0, size-1]` internally).
 * @return          Index of the first bar in the same UTC day as `bars[idx]`.
 *                  Returns `0` when `bars` is NULL or `size` is 0.
 */
size_t cxta_series_session_start_index(const cxta_series_bar* bars, size_t size, size_t idx);

/**
 * @brief Highest high over the first `period` bars of the current UTC session.
 *
 * Returns `NAN` when the current bar is still within the opening range
 * (i.e. fewer than `period` bars have elapsed since the session start).
 *
 * @param[in] bars   Bar array.
 * @param[in] size   Number of bars.
 * @param[in] idx    Current bar index.
 * @param[in] period Opening range length in bars (>= 1).
 * @return           Max high over `[session_start, session_start + period)`,
 *                   or `NAN` if still within the opening range.
 */
double cxta_series_session_high(const cxta_series_bar* bars, size_t size, size_t idx, size_t period);

/**
 * @brief Lowest low over the first `period` bars of the current UTC session.
 *
 * Returns `NAN` when the current bar is still within the opening range.
 *
 * @param[in] bars   Bar array.
 * @param[in] size   Number of bars.
 * @param[in] idx    Current bar index.
 * @param[in] period Opening range length in bars (>= 1).
 * @return           Min low over `[session_start, session_start + period)`,
 *                   or `NAN` if still within the opening range.
 */
double cxta_series_session_low(const cxta_series_bar* bars, size_t size, size_t idx, size_t period);

/**
 * @brief Construct a clamped SoA view.
 */
cxta_series_soa_view cxta_series_soa_view_make(const double* open,
                               const double* high,
                               const double* low,
                               const double* close,
                               const double* volume,
                               size_t size,
                               size_t index);
/**
 * @brief Validate a SoA view.
 */
int cxta_series_soa_view_valid(const cxta_series_soa_view* view);
/**
 * @brief Read close value at index from SoA view.
 */
int cxta_series_soa_close_at(const cxta_series_soa_view* view, size_t i, double* out);
/**
 * @brief Read current close value from SoA view.
 */
int cxta_series_soa_close_current(const cxta_series_soa_view* view, double* out);

/**
 * @brief Construct a clamped scalar view.
 */
cxta_series_scalar_view cxta_series_scalar_view_make(const double* values, size_t size, size_t index);
/**
 * @brief Validate a scalar view.
 */
int cxta_series_scalar_view_valid(const cxta_series_scalar_view* view);
/**
 * @brief Read scalar value at index from scalar view.
 */
int cxta_series_scalar_at(const cxta_series_scalar_view* view, size_t i, double* out);
/**
 * @brief Read current scalar value from scalar view.
 */
int cxta_series_scalar_current(const cxta_series_scalar_view* view, double* out);

#ifdef __cplusplus
}
#endif
