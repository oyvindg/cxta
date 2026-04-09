/**
 * @file tf.h
 * @brief Multi-timeframe OHLCV and timestamp accessors.
 */

#pragma once

#include <cxta/series/bar.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Read the current open from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Current open value, or `NAN` when resolution fails.
 */
double cxta_ts_open_tf(uint64_t handle, const cxta_series_resolver* resolver);

/**
 * @brief Read the current high from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Current high value, or `NAN` when resolution fails.
 */
double cxta_ts_high_tf(uint64_t handle, const cxta_series_resolver* resolver);

/**
 * @brief Read the current low from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Current low value, or `NAN` when resolution fails.
 */
double cxta_ts_low_tf(uint64_t handle, const cxta_series_resolver* resolver);

/**
 * @brief Read the current close from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Current close value, or `NAN` when resolution fails.
 */
double cxta_ts_close_tf(uint64_t handle, const cxta_series_resolver* resolver);

/**
 * @brief Read the current volume from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Current volume value, or `NAN` when resolution fails.
 */
double cxta_ts_volume_tf(uint64_t handle, const cxta_series_resolver* resolver);

/**
 * @brief Read the current timestamp from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Current timestamp, or `0` when resolution fails.
 */
uint64_t cxta_ts_timestamp_tf(uint64_t handle, const cxta_series_resolver* resolver);

/**
 * @brief Read an open value at a lookback offset from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] offset Lookback bars from the aligned current bar (`0` = current).
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Open value at `offset`, or `NAN` when resolution fails.
 */
double cxta_ts_at_open_tf(uint64_t handle, size_t offset, const cxta_series_resolver* resolver);

/**
 * @brief Read a high value at a lookback offset from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] offset Lookback bars from the aligned current bar (`0` = current).
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return High value at `offset`, or `NAN` when resolution fails.
 */
double cxta_ts_at_high_tf(uint64_t handle, size_t offset, const cxta_series_resolver* resolver);

/**
 * @brief Read a low value at a lookback offset from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] offset Lookback bars from the aligned current bar (`0` = current).
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Low value at `offset`, or `NAN` when resolution fails.
 */
double cxta_ts_at_low_tf(uint64_t handle, size_t offset, const cxta_series_resolver* resolver);

/**
 * @brief Read a close value at a lookback offset from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] offset Lookback bars from the aligned current bar (`0` = current).
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Close value at `offset`, or `NAN` when resolution fails.
 */
double cxta_ts_at_close_tf(uint64_t handle, size_t offset, const cxta_series_resolver* resolver);

/**
 * @brief Read a volume value at a lookback offset from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] offset Lookback bars from the aligned current bar (`0` = current).
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Volume value at `offset`, or `NAN` when resolution fails.
 */
double cxta_ts_at_volume_tf(uint64_t handle, size_t offset, const cxta_series_resolver* resolver);

/**
 * @brief Read a timestamp at a lookback offset from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] offset Lookback bars from the aligned current bar (`0` = current).
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Timestamp at `offset`, or `0` when resolution fails.
 */
uint64_t cxta_ts_at_timestamp_tf(uint64_t handle, size_t offset, const cxta_series_resolver* resolver);

#ifdef __cplusplus
}
#endif
