/**
 * @file tf.c
 * @brief Multi-timeframe OHLCV and timestamp accessors.
 */

#include <cxta/ts/tf.h>
#include <math.h>

typedef double (*cxta_tf_double_reader_fn)(const cxta_series_bar* bar);
typedef uint64_t (*cxta_tf_u64_reader_fn)(const cxta_series_bar* bar);

static int cxta_tf_resolve_view(uint64_t handle,
                                const cxta_series_resolver* resolver,
                                cxta_series_bar_view* out) {
    if (!out || !resolver || !resolver->resolve) return 0;
    return resolver->resolve(handle, out, resolver->userdata);
}

static const cxta_series_bar* cxta_tf_bar_at(uint64_t handle,
                                             const cxta_series_resolver* resolver,
                                             size_t (*index_fn)(size_t, size_t, size_t),
                                             size_t lag) {
    cxta_series_bar_view view;
    size_t index;

    if (!cxta_tf_resolve_view(handle, resolver, &view)) return NULL;
    if (!view.bars || view.size == 0u) return NULL;

    if (index_fn) {
        index = index_fn(view.size, view.index, lag);
    } else {
        index = cxta_series_clamp_index(view.size, view.index);
    }
    return &view.bars[index];
}

static double cxta_tf_read_double(uint64_t handle,
                                  const cxta_series_resolver* resolver,
                                  size_t offset,
                                  cxta_tf_double_reader_fn reader) {
    const cxta_series_bar* bar = cxta_tf_bar_at(handle, resolver, cxta_series_lag_index, offset);
    if (!bar || !reader) return NAN;
    return reader(bar);
}

static uint64_t cxta_tf_read_u64(uint64_t handle,
                                 const cxta_series_resolver* resolver,
                                 size_t offset,
                                 cxta_tf_u64_reader_fn reader) {
    const cxta_series_bar* bar = cxta_tf_bar_at(handle, resolver, cxta_series_lag_index, offset);
    if (!bar || !reader) return 0u;
    return reader(bar);
}

static double cxta_tf_read_open(const cxta_series_bar* bar) { return bar ? bar->open : NAN; }
static double cxta_tf_read_high(const cxta_series_bar* bar) { return bar ? bar->high : NAN; }
static double cxta_tf_read_low(const cxta_series_bar* bar) { return bar ? bar->low : NAN; }
static double cxta_tf_read_close(const cxta_series_bar* bar) { return bar ? bar->close : NAN; }
static double cxta_tf_read_volume(const cxta_series_bar* bar) { return bar ? bar->volume : NAN; }
static uint64_t cxta_tf_read_timestamp(const cxta_series_bar* bar) {
    return bar ? bar->timestamp : 0u;
}

double cxta_ts_open_tf(uint64_t handle, const cxta_series_resolver* resolver) {
    return cxta_ts_at_open_tf(handle, 0u, resolver);
}

double cxta_ts_high_tf(uint64_t handle, const cxta_series_resolver* resolver) {
    return cxta_ts_at_high_tf(handle, 0u, resolver);
}

double cxta_ts_low_tf(uint64_t handle, const cxta_series_resolver* resolver) {
    return cxta_ts_at_low_tf(handle, 0u, resolver);
}

double cxta_ts_close_tf(uint64_t handle, const cxta_series_resolver* resolver) {
    return cxta_ts_at_close_tf(handle, 0u, resolver);
}

double cxta_ts_volume_tf(uint64_t handle, const cxta_series_resolver* resolver) {
    return cxta_ts_at_volume_tf(handle, 0u, resolver);
}

uint64_t cxta_ts_timestamp_tf(uint64_t handle, const cxta_series_resolver* resolver) {
    return cxta_ts_at_timestamp_tf(handle, 0u, resolver);
}

double cxta_ts_at_open_tf(uint64_t handle, size_t offset, const cxta_series_resolver* resolver) {
    return cxta_tf_read_double(handle, resolver, offset, cxta_tf_read_open);
}

double cxta_ts_at_high_tf(uint64_t handle, size_t offset, const cxta_series_resolver* resolver) {
    return cxta_tf_read_double(handle, resolver, offset, cxta_tf_read_high);
}

double cxta_ts_at_low_tf(uint64_t handle, size_t offset, const cxta_series_resolver* resolver) {
    return cxta_tf_read_double(handle, resolver, offset, cxta_tf_read_low);
}

double cxta_ts_at_close_tf(uint64_t handle, size_t offset, const cxta_series_resolver* resolver) {
    return cxta_tf_read_double(handle, resolver, offset, cxta_tf_read_close);
}

double cxta_ts_at_volume_tf(uint64_t handle, size_t offset, const cxta_series_resolver* resolver) {
    return cxta_tf_read_double(handle, resolver, offset, cxta_tf_read_volume);
}

uint64_t cxta_ts_at_timestamp_tf(uint64_t handle,
                                 size_t offset,
                                 const cxta_series_resolver* resolver) {
    return cxta_tf_read_u64(handle, resolver, offset, cxta_tf_read_timestamp);
}
