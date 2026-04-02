/**
 * @file volume_profile.c
 * @brief Rolling volume-profile helpers.
 */

#include <cxta/indicators/volume_profile.h>
#include <cxta/ts/smoothing.h>

static double cxta_volume_profile_max2(double a, double b) {
    return (a > b) ? a : b;
}

static double cxta_volume_profile_min2(double a, double b) {
    return (a < b) ? a : b;
}

cxta_vp_output cxta_volume_profile(const cxta_series_bar_view* view,
                                   int period,
                                   int bins) {
    cxta_vp_output out = {0.0, 0.0, 0.0, 0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    if (bins < 5) bins = 5;
    if (bins > 200) bins = 200;

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        const size_t p = (size_t)cxta_ts_clamp_period(period);
        const size_t window = (p < (idx + 1)) ? p : (idx + 1);
        const size_t start = idx + 1 - window;
        double range_high = view->bars[start].high;
        double range_low = view->bars[start].low;
        double bucket_volume[200] = {0.0};
        double total_volume = 0.0;

        for (size_t i = start + 1; i <= idx; ++i) {
            range_high = cxta_volume_profile_max2(range_high, view->bars[i].high);
            range_low = cxta_volume_profile_min2(range_low, view->bars[i].low);
        }

        out.range_high = range_high;
        out.range_low = range_low;

        {
            const double width = range_high - range_low;
            if (width <= 1e-12) {
                const double c = view->bars[idx].close;
                out.poc = c;
                out.vah = c;
                out.val = c;
                return out;
            }

            for (size_t i = start; i <= idx; ++i) {
                const double tp = cxta_series_typical_price(&view->bars[i]);
                double norm = (tp - range_low) / width;
                if (norm < 0.0) norm = 0.0;
                if (norm >= 1.0) norm = 0.999999999;

                {
                    const int bucket = (int)(norm * (double)bins);
                    const double volume = (view->bars[i].volume > 0.0) ? view->bars[i].volume : 0.0;
                    bucket_volume[bucket] += volume;
                    total_volume += volume;
                }
            }

            {
                int poc_idx = 0;
                int left = 0;
                int right = 0;
                double accumulated = 0.0;
                const double target = total_volume * 0.70;
                const double bin_width = width / (double)bins;

                for (int i = 1; i < bins; ++i) {
                    if (bucket_volume[i] > bucket_volume[poc_idx]) poc_idx = i;
                }

                left = poc_idx;
                right = poc_idx;
                accumulated = bucket_volume[poc_idx];

                while (accumulated < target && (left > 0 || right + 1 < bins)) {
                    const double left_next = (left > 0) ? bucket_volume[left - 1] : -1.0;
                    const double right_next = (right + 1 < bins) ? bucket_volume[right + 1] : -1.0;
                    if (right_next > left_next) {
                        ++right;
                        accumulated += bucket_volume[right];
                    } else if (left > 0) {
                        --left;
                        accumulated += bucket_volume[left];
                    } else {
                        ++right;
                        accumulated += bucket_volume[right];
                    }
                }

                out.poc = range_low + ((double)poc_idx + 0.5) * bin_width;
                out.val = range_low + (double)left * bin_width;
                out.vah = range_low + (double)(right + 1) * bin_width;
            }
        }
    }
    return out;
}
