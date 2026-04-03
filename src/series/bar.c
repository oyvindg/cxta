/**
 * @file bar.c
 * @brief Bar-series views and index helpers.
 */

#include <cxta/series/bar.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

size_t cxta_series_clamp_index(size_t size, size_t index) {
    if (size == 0) return 0;
    return (index < size) ? index : (size - 1);
}

size_t cxta_series_prev_index(size_t size, size_t index) {
    if (size == 0) return 0;
    const size_t clamped = cxta_series_clamp_index(size, index);
    if (clamped == 0) return 0;
    return clamped - 1;
}

size_t cxta_series_lag_index(size_t size, size_t index, size_t lag) {
    if (size == 0) return 0;
    const size_t clamped = cxta_series_clamp_index(size, index);
    const size_t raw = (clamped > lag) ? (clamped - lag) : 0;
    return cxta_series_clamp_index(size, raw);
}

cxta_series_bar_view cxta_series_bar_view_make(const cxta_series_bar* bars, size_t size, size_t index) {
    cxta_series_bar_view view;
    view.bars = bars;
    view.size = size;
    view.index = cxta_series_clamp_index(size, index);
    return view;
}

int cxta_series_bar_view_valid(const cxta_series_bar_view* view) {
    if (!view) return 0;
    if (!view->bars || view->size == 0) return 0;
    return (view->index < view->size) ? 1 : 0;
}

const cxta_series_bar* cxta_series_bar_view_at(const cxta_series_bar_view* view, size_t i) {
    if (!cxta_series_bar_view_valid(view)) return 0;
    if (i >= view->size) return 0;
    return &view->bars[i];
}

const cxta_series_bar* cxta_series_bar_view_current(const cxta_series_bar_view* view) {
    if (!cxta_series_bar_view_valid(view)) return 0;
    return &view->bars[view->index];
}

void cxta_series_bar_buffer_init(cxta_series_bar_buffer* buffer) {
    if (!buffer) return;
    buffer->data = 0;
    buffer->size = 0;
    buffer->capacity = 0;
}

void cxta_series_bar_buffer_free(cxta_series_bar_buffer* buffer) {
    if (!buffer) return;
    free(buffer->data);
    cxta_series_bar_buffer_init(buffer);
}

int cxta_series_bar_buffer_reserve(cxta_series_bar_buffer* buffer, size_t capacity) {
    cxta_series_bar* next = 0;
    if (!buffer) return 0;
    if (capacity <= buffer->capacity) return 1;
    next = (cxta_series_bar*)realloc(buffer->data, capacity * sizeof(cxta_series_bar));
    if (!next) return 0;
    buffer->data = next;
    buffer->capacity = capacity;
    return 1;
}

void cxta_series_bar_buffer_clear(cxta_series_bar_buffer* buffer) {
    if (!buffer) return;
    buffer->size = 0;
}

int cxta_series_bar_buffer_push(cxta_series_bar_buffer* buffer, cxta_series_bar bar) {
    size_t next_capacity = 0;
    if (!buffer) return 0;
    if (buffer->size == buffer->capacity) {
        next_capacity = (buffer->capacity == 0u) ? 8u : (buffer->capacity * 2u);
        if (next_capacity < buffer->size + 1u) {
            next_capacity = buffer->size + 1u;
        }
        if (!cxta_series_bar_buffer_reserve(buffer, next_capacity)) return 0;
    }
    buffer->data[buffer->size++] = bar;
    return 1;
}

cxta_series_bar_view cxta_series_bar_buffer_view(const cxta_series_bar_buffer* buffer, size_t index) {
    if (!buffer) return cxta_series_bar_view_make(0, 0, 0);
    return cxta_series_bar_view_make(buffer->data, buffer->size, index);
}

double cxta_series_typical_price(const cxta_series_bar* bar) {
    if (!bar) return 0.0;
    return (bar->high + bar->low + bar->close) / 3.0;
}

cxta_series_soa_view cxta_series_soa_view_make(const double* open,
                               const double* high,
                               const double* low,
                               const double* close,
                               const double* volume,
                               size_t size,
                               size_t index) {
    cxta_series_soa_view view;
    view.open = open;
    view.high = high;
    view.low = low;
    view.close = close;
    view.volume = volume;
    view.size = size;
    view.index = cxta_series_clamp_index(size, index);
    return view;
}

int cxta_series_soa_view_valid(const cxta_series_soa_view* view) {
    if (!view) return 0;
    if (view->size == 0) return 0;
    if (!view->open || !view->high || !view->low || !view->close || !view->volume) return 0;
    return (view->index < view->size) ? 1 : 0;
}

int cxta_series_soa_close_at(const cxta_series_soa_view* view, size_t i, double* out) {
    if (!out) return 0;
    if (!cxta_series_soa_view_valid(view)) return 0;
    if (i >= view->size) return 0;
    *out = view->close[i];
    return 1;
}

int cxta_series_soa_close_current(const cxta_series_soa_view* view, double* out) {
    if (!out) return 0;
    if (!cxta_series_soa_view_valid(view)) return 0;
    *out = view->close[view->index];
    return 1;
}

cxta_series_scalar_view cxta_series_scalar_view_make(const double* values, size_t size, size_t index) {
    cxta_series_scalar_view view;
    view.values = values;
    view.size = size;
    view.index = cxta_series_clamp_index(size, index);
    return view;
}

int cxta_series_scalar_view_valid(const cxta_series_scalar_view* view) {
    if (!view) return 0;
    if (!view->values || view->size == 0) return 0;
    return (view->index < view->size) ? 1 : 0;
}

int cxta_series_scalar_at(const cxta_series_scalar_view* view, size_t i, double* out) {
    if (!out) return 0;
    if (!cxta_series_scalar_view_valid(view)) return 0;
    if (i >= view->size) return 0;
    *out = view->values[i];
    return 1;
}

int cxta_series_scalar_current(const cxta_series_scalar_view* view, double* out) {
    if (!out) return 0;
    if (!cxta_series_scalar_view_valid(view)) return 0;
    *out = view->values[view->index];
    return 1;
}

size_t cxta_series_session_start_index(const cxta_series_bar* bars, size_t size, size_t idx) {
    if (!bars || size == 0) return 0;
    if (idx >= size) idx = size - 1;
    const uint64_t current_day = bars[idx].timestamp / 86400ULL;
    while (idx > 0 && bars[idx - 1].timestamp / 86400ULL == current_day) {
        --idx;
    }
    return idx;
}

double cxta_series_session_high(const cxta_series_bar* bars, size_t size, size_t idx, size_t period) {
    if (!bars || size == 0 || period == 0) return NAN;
    if (idx >= size) idx = size - 1;
    const size_t start = cxta_series_session_start_index(bars, size, idx);
    if (idx - start < period) return NAN;
    double h = bars[start].high;
    for (size_t i = start + 1; i < start + period; ++i) {
        if (bars[i].high > h) h = bars[i].high;
    }
    return h;
}

double cxta_series_session_low(const cxta_series_bar* bars, size_t size, size_t idx, size_t period) {
    if (!bars || size == 0 || period == 0) return NAN;
    if (idx >= size) idx = size - 1;
    const size_t start = cxta_series_session_start_index(bars, size, idx);
    if (idx - start < period) return NAN;
    double l = bars[start].low;
    for (size_t i = start + 1; i < start + period; ++i) {
        if (bars[i].low < l) l = bars[i].low;
    }
    return l;
}
