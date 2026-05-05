/**
 * @file fair_value_gap.c
 * @brief Persistent Fair Value Gap helpers.
 */

#include <cxta/indicators/fair_value_gap.h>
#include <cxta/indicators/macros.h>

#include <math.h>
#include <stddef.h>

typedef struct {
    double gap_high;
    double gap_low;
    double direction;
    size_t formed_index;
} cxta_fair_value_gap_state;

static int cxta_fair_value_gap_bar_touches(const cxta_series_bar* bar,
                                           const cxta_fair_value_gap_state* gap) {
    return bar != NULL &&
           gap != NULL &&
           bar->high >= gap->gap_low &&
           bar->low <= gap->gap_high;
}

static double cxta_fair_value_gap_fill_pct(const cxta_fair_value_gap_state* gap,
                                           double close) {
    double width;
    if (!gap) return 0.0;
    width = gap->gap_high - gap->gap_low;
    if (!(width > 0.0) || !isfinite(width)) return 0.0;

    if (gap->direction > 0.0) {
        if (close >= gap->gap_high) return 0.0;
        if (close <= gap->gap_low) return 1.0;
        return fmin(fmax((gap->gap_high - close) / width, 0.0), 1.0);
    }

    if (gap->direction < 0.0) {
        if (close <= gap->gap_low) return 0.0;
        if (close >= gap->gap_high) return 1.0;
        return fmin(fmax((close - gap->gap_low) / width, 0.0), 1.0);
    }

    return 0.0;
}

static void cxta_fair_value_gap_populate(cxta_fair_value_gap_output* out,
                                         const cxta_fair_value_gap_state* gap,
                                         double close,
                                         double mitigated) {
    if (!out || !gap) return;
    out->gap_high = gap->gap_high;
    out->gap_low = gap->gap_low;
    out->direction = gap->direction;
    out->mitigated = mitigated;
    out->fill_pct = cxta_fair_value_gap_fill_pct(gap, close);
}

static const cxta_field_descriptor cxta_fair_value_gap_fields[] = {
    {"gap_high", offsetof(cxta_fair_value_gap_output, gap_high), false},
    {"gap_low", offsetof(cxta_fair_value_gap_output, gap_low), false},
    {"direction", offsetof(cxta_fair_value_gap_output, direction), true},
    {"mitigated", offsetof(cxta_fair_value_gap_output, mitigated), true},
    {"fill_pct", offsetof(cxta_fair_value_gap_output, fill_pct), true},
};

static const cxta_plot_field_descriptor cxta_fair_value_gap_plot_fields[] = {
    CXTA_FIELD_PLOT("gap_high", false, "FVG High", "fvg", "#22c55e", "line", "price", "High boundary of the active fair value gap.", "Use with gap_low to draw the imbalance zone."),
    CXTA_FIELD_PLOT("gap_low", false, "FVG Low", "fvg", "#ef4444", "line", "price", "Low boundary of the active fair value gap.", "Use with gap_high to draw the imbalance zone."),
    CXTA_FIELD_PLOT("direction", true, "FVG Direction", "fvg", "#38bdf8", "hidden", "fvg", "Fair value gap direction.", "Positive is bullish imbalance; negative is bearish imbalance."),
    CXTA_FIELD_PLOT("mitigated", true, "FVG Mitigated", "fvg", "#f59e0b", "hidden", "fvg", "Mitigation state for the active gap.", "Use transitions to detect filled or invalidated imbalance zones."),
    CXTA_FIELD_PLOT("fill_pct", true, "FVG Fill %", "fvg", "#a78bfa", "line", "fvg", "Current fill percentage of the active gap.", "Higher values indicate more of the imbalance has been filled."),
};

static const cxta_indicator_plot_descriptor cxta_fair_value_gap_plot_descriptor = {
    .indicator_name = "fair_value_gap",
    .fields = cxta_fair_value_gap_plot_fields,
    .field_count = CXTA_ARRAY_COUNT(cxta_fair_value_gap_plot_fields),
};

static void cxta_fair_value_gap_descriptor_eval(const cxta_series_bar_view* view,
                                                const double* args,
                                                size_t nargs,
                                                void* out) {
    cxta_fair_value_gap_output value = {0};
    (void)args;
    (void)nargs;

    if (view && cxta_series_bar_view_valid(view)) {
        value = cxta_fair_value_gap(view);
    }
    if (out) {
        *((cxta_fair_value_gap_output*)out) = value;
    }
}

const cxta_indicator_descriptor cxta_fair_value_gap_descriptor = {
    "fair_value_gap",
    0,
    1,
    -1,
    -1,
    2,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_fair_value_gap_output),
    0u,
    cxta_fair_value_gap_fields,
    CXTA_ARRAY_COUNT(cxta_fair_value_gap_fields),
    NULL,
    cxta_fair_value_gap_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0u,
    "fvg",
    &cxta_fair_value_gap_plot_descriptor,
};

cxta_fair_value_gap_output cxta_fair_value_gap(const cxta_series_bar_view* view) {
    cxta_fair_value_gap_output out = {0.0, 0.0, 0.0, 0.0, 0.0};
    cxta_fair_value_gap_state active_gap = {0.0, 0.0, 0.0, 0u};
    cxta_fair_value_gap_state mitigated_gap = {0.0, 0.0, 0.0, 0u};
    int has_active_gap = 0;
    int has_mitigated_gap = 0;
    size_t i;
    size_t current_index;
    const cxta_series_bar* current_bar;

    if (!view || !cxta_series_bar_view_valid(view)) return out;

    current_index = cxta_series_clamp_index(view->size, view->index);
    if (current_index < 2u) return out;

    for (i = 2u; i <= current_index; ++i) {
        const cxta_series_bar* left;
        const cxta_series_bar* right;

        if (has_active_gap && i > active_gap.formed_index) {
            const cxta_series_bar* bar = cxta_series_bar_view_at(view, i);
            if (cxta_fair_value_gap_bar_touches(bar, &active_gap)) {
                if (i == current_index) {
                    mitigated_gap = active_gap;
                    has_mitigated_gap = 1;
                }
                has_active_gap = 0;
            }
        }

        left = cxta_series_bar_view_at(view, i - 2u);
        right = cxta_series_bar_view_at(view, i);
        if (!left || !right) continue;

        if (right->low > left->high) {
            active_gap.gap_high = right->low;
            active_gap.gap_low = left->high;
            active_gap.direction = 1.0;
            active_gap.formed_index = i;
            has_active_gap = 1;
            continue;
        }

        if (right->high < left->low) {
            active_gap.gap_high = left->low;
            active_gap.gap_low = right->high;
            active_gap.direction = -1.0;
            active_gap.formed_index = i;
            has_active_gap = 1;
        }
    }

    current_bar = cxta_series_bar_view_current(view);
    if (!current_bar) return out;

    if (has_active_gap) {
        cxta_fair_value_gap_populate(&out, &active_gap, current_bar->close, 0.0);
        return out;
    }

    if (has_mitigated_gap) {
        cxta_fair_value_gap_populate(&out, &mitigated_gap, current_bar->close, 1.0);
    }

    return out;
}
