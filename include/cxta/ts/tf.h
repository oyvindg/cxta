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
 * @brief Read the previous open from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Previous open value, or `NAN` when resolution fails.
 */
double cxta_ts_prev_open_tf(uint64_t handle, const cxta_series_resolver* resolver);

/**
 * @brief Read the previous high from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Previous high value, or `NAN` when resolution fails.
 */
double cxta_ts_prev_high_tf(uint64_t handle, const cxta_series_resolver* resolver);

/**
 * @brief Read the previous low from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Previous low value, or `NAN` when resolution fails.
 */
double cxta_ts_prev_low_tf(uint64_t handle, const cxta_series_resolver* resolver);

/**
 * @brief Read the previous close from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Previous close value, or `NAN` when resolution fails.
 */
double cxta_ts_prev_close_tf(uint64_t handle, const cxta_series_resolver* resolver);

/**
 * @brief Read the previous volume from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Previous volume value, or `NAN` when resolution fails.
 */
double cxta_ts_prev_volume_tf(uint64_t handle, const cxta_series_resolver* resolver);

/**
 * @brief Read the previous timestamp from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Previous timestamp, or `0` when resolution fails.
 */
uint64_t cxta_ts_prev_timestamp_tf(uint64_t handle, const cxta_series_resolver* resolver);

/**
 * @brief Read a lagged open from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] lag Number of bars to look back.
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Lagged open value, or `NAN` when resolution fails.
 */
double cxta_ts_lag_open_tf(uint64_t handle, size_t lag, const cxta_series_resolver* resolver);

/**
 * @brief Read a lagged high from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] lag Number of bars to look back.
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Lagged high value, or `NAN` when resolution fails.
 */
double cxta_ts_lag_high_tf(uint64_t handle, size_t lag, const cxta_series_resolver* resolver);

/**
 * @brief Read a lagged low from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] lag Number of bars to look back.
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Lagged low value, or `NAN` when resolution fails.
 */
double cxta_ts_lag_low_tf(uint64_t handle, size_t lag, const cxta_series_resolver* resolver);

/**
 * @brief Read a lagged close from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] lag Number of bars to look back.
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Lagged close value, or `NAN` when resolution fails.
 */
double cxta_ts_lag_close_tf(uint64_t handle, size_t lag, const cxta_series_resolver* resolver);

/**
 * @brief Read a lagged volume from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] lag Number of bars to look back.
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Lagged volume value, or `NAN` when resolution fails.
 */
double cxta_ts_lag_volume_tf(uint64_t handle, size_t lag, const cxta_series_resolver* resolver);

/**
 * @brief Read a lagged timestamp from a resolved timeframe series.
 * @param[in] handle Host-issued timeframe handle. `0` may mean primary series.
 * @param[in] lag Number of bars to look back.
 * @param[in] resolver Resolver callback bundle used to map handle to a bar view.
 * @return Lagged timestamp, or `0` when resolution fails.
 */
uint64_t cxta_ts_lag_timestamp_tf(uint64_t handle, size_t lag, const cxta_series_resolver* resolver);

#ifdef __cplusplus
}
#endif
