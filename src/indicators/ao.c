/**
 * @file ao.c
 * @brief Awesome Oscillator helpers.
 */

#include <cxta/indicators/ao.h>
#include <cxta/ts/smoothing.h>

static double midpoint_sma(const cxta_series_bar* bars, size_t start, size_t end) {
    double sum = 0.0;
    size_t count = end - start + 1;
    for (size_t i = start; i <= end; ++i) {
        sum += (bars[i].high + bars[i].low) * 0.5;
    }
    return (count > 0) ? (sum / (double)count) : 0.0;
}

double cxta_ao(const cxta_series_bar_view* view) {
    if (!view || !cxta_series_bar_view_valid(view)) return 0.0;

    const size_t idx = cxta_series_clamp_index(view->size, view->index);
    const size_t w5  = (5  < (idx + 1)) ? 5  : (idx + 1);
    const size_t w34 = (34 < (idx + 1)) ? 34 : (idx + 1);

    const double sma5  = midpoint_sma(view->bars, idx + 1 - w5,  idx);
    const double sma34 = midpoint_sma(view->bars, idx + 1 - w34, idx);
    return sma5 - sma34;
}
