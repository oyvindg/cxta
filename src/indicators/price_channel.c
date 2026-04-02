/**
 * @file price_channel.c
 * @brief Price channel helpers.
 */

#include <cxta/indicators/price_channel.h>
#include <cxta/indicators/donchian.h>

cxta_channel_output cxta_price_channel(const cxta_series_bar_view* view, int period) {
    return cxta_donchian(view, period);
}
