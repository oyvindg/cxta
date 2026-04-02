#include <cxta/ts/tf.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>

static const cxta_series_bar g_bars_1m[] = {
    {1000u, 1.0, 2.0, 0.5, 1.0, 100.0},
    {1060u, 1.2, 2.2, 0.7, 1.5, 110.0},
};

static const cxta_series_bar g_bars_4h[] = {
    {2000u, 10.0, 20.0, 5.0, 12.0, 200.0},
    {2000u + 14400u, 11.0, 21.0, 6.0, 15.0, 220.0},
};

static int stub_resolver(uint64_t handle, cxta_series_bar_view* out, void* userdata) {
    (void)userdata;
    if (!out) return 0;
    if (handle == 1u) {
        *out = cxta_series_bar_view_make(g_bars_1m, 2u, 1u);
        return 1;
    }
    if (handle == 2u) {
        *out = cxta_series_bar_view_make(g_bars_4h, 2u, 1u);
        return 1;
    }
    return 0;
}

void cxta_test_tf(void) {
    const cxta_series_resolver resolver = {stub_resolver, NULL};

    assert(cxta_ts_open_tf(1u, &resolver) == 1.2);
    assert(cxta_ts_high_tf(1u, &resolver) == 2.2);
    assert(cxta_ts_low_tf(1u, &resolver) == 0.7);
    assert(cxta_ts_close_tf(1u, &resolver) == 1.5);
    assert(cxta_ts_volume_tf(1u, &resolver) == 110.0);
    assert(cxta_ts_timestamp_tf(1u, &resolver) == 1060u);

    assert(cxta_ts_prev_open_tf(1u, &resolver) == 1.0);
    assert(cxta_ts_prev_high_tf(1u, &resolver) == 2.0);
    assert(cxta_ts_prev_low_tf(1u, &resolver) == 0.5);
    assert(cxta_ts_prev_close_tf(1u, &resolver) == 1.0);
    assert(cxta_ts_prev_volume_tf(1u, &resolver) == 100.0);
    assert(cxta_ts_prev_timestamp_tf(1u, &resolver) == 1000u);

    assert(cxta_ts_lag_open_tf(1u, 1u, &resolver) == 1.0);
    assert(cxta_ts_lag_high_tf(1u, 1u, &resolver) == 2.0);
    assert(cxta_ts_lag_low_tf(1u, 1u, &resolver) == 0.5);
    assert(cxta_ts_lag_close_tf(1u, 1u, &resolver) == 1.0);
    assert(cxta_ts_lag_volume_tf(1u, 1u, &resolver) == 100.0);
    assert(cxta_ts_lag_timestamp_tf(1u, 1u, &resolver) == 1000u);

    assert(cxta_ts_close_tf(2u, &resolver) == 15.0);
    assert(cxta_ts_lag_close_tf(2u, 9u, &resolver) == 12.0);

    assert(isnan(cxta_ts_close_tf(99u, &resolver)));
    assert(isnan(cxta_ts_prev_close_tf(99u, &resolver)));
    assert(isnan(cxta_ts_lag_close_tf(99u, 1u, &resolver)));
    assert(cxta_ts_timestamp_tf(99u, &resolver) == 0u);
    assert(cxta_ts_prev_timestamp_tf(99u, &resolver) == 0u);
    assert(cxta_ts_lag_timestamp_tf(99u, 1u, &resolver) == 0u);

    printf("  OK tf\n");
}
