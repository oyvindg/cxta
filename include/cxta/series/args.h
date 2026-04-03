/**
 * @file args.h
 * @brief Argument tuple parsers for cxta bar series helpers.
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Parsed OHLC argument tuple from function call args.
 */
typedef struct {
    double open;
    double high;
    double low;
    double close;
} cxta_series_bar_args;

/**
 * @brief Parsed current/previous open-close tuple from function call args.
 */
typedef struct {
    double open;
    double close;
    double prev_open;
    double prev_close;
} cxta_series_oc_pair_args;

/**
 * @brief Parse first four scalar args as OHLC tuple.
 */
int cxta_series_bar_args_parse(const double* args, size_t nargs, cxta_series_bar_args* out);
/**
 * @brief Parse first four scalar args as `(open, close, prev_open, prev_close)`.
 */
int cxta_series_oc_pair_args_parse(const double* args, size_t nargs, cxta_series_oc_pair_args* out);

#ifdef __cplusplus
}
#endif
