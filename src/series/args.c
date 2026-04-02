/**
 * @file args.c
 * @brief Argument parsers for cxseries-related helpers.
 */

#include <cxta/series/args.h>

int cxta_series_bar_args_parse(const double* args, size_t nargs, cxta_series_bar_args* out) {
    if (!out || !args || nargs < 4) return 0;
    out->open = args[0];
    out->high = args[1];
    out->low = args[2];
    out->close = args[3];
    return 1;
}

int cxta_series_oc_pair_args_parse(const double* args, size_t nargs, cxta_series_oc_pair_args* out) {
    if (!out || !args || nargs < 4) return 0;
    out->open = args[0];
    out->close = args[1];
    out->prev_open = args[2];
    out->prev_close = args[3];
    return 1;
}

