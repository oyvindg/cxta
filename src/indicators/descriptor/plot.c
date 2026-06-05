/**
 * @file plot.c
 * @brief Descriptor plot metadata and fallback helpers.
 */

#include <cxta/cxta.h>

#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    const char* name;
    cxta_scalar_plot_descriptor plot;
} cxta_builtin_plot_entry;

static const cxta_builtin_plot_entry builtin_plot_descriptors[] = {
    {
        "open",
        {true, "Open", "price", "#94a3b8", "line", "price", NULL, NULL},
    },
    {
        "high",
        {true, "High", "price", "#22c55e", "line", "price", NULL, NULL},
    },
    {
        "low",
        {true, "Low", "price", "#ef4444", "line", "price", NULL, NULL},
    },
    {
        "close",
        {true, "Close", "price", "#93c5fd", "line", "price", NULL, NULL},
    },
    {
        "volume",
        {true, "Volume", "price", "#94a3b8", "histogram", "volume", "#22c55e", "#ef4444"},
    },
};

static const char* cxta_descriptor_scalar_pane_for_name(const char* indicator_name) {
    (void)indicator_name;
    return "indicator";
}

static int cxta_descriptor_name_is_acronym(const char* name, size_t start, size_t end) {
    static const char* const acronyms[] = {
        "adx", "ao", "atr", "bop", "cci", "cmf", "cmo", "crsi", "dpo", "ema",
        "fvg", "hma", "kst", "mfi", "nvi", "ppo", "pvi", "rma", "rsi",
        "rvi", "sfp", "sma", "tsi", "vwap", "wma",
    };
    char token[32];
    size_t i;
    size_t n = end > start ? end - start : 0u;

    if (!name || n == 0u || n >= sizeof(token)) return 0;
    for (i = 0u; i < n; ++i) token[i] = name[start + i];
    token[n] = '\0';
    for (i = 0u; i < CXTA_ARRAY_COUNT(acronyms); ++i) {
        if (strcmp(token, acronyms[i]) == 0) return 1;
    }
    return 0;
}

static void cxta_descriptor_make_label(const char* name, char* out, size_t out_size) {
    size_t i = 0u;
    size_t o = 0u;
    size_t word_start = 0u;

    if (!out || out_size == 0u) return;
    out[0] = '\0';
    if (!name || name[0] == '\0') return;

    while (name[i] != '\0' && o + 1u < out_size) {
        while (name[i] == '_') ++i;
        if (name[i] == '\0') break;
        word_start = i;
        while (name[i] != '\0' && name[i] != '_') ++i;

        if (o > 0u && o + 1u < out_size) out[o++] = ' ';
        if (cxta_descriptor_name_is_acronym(name, word_start, i)) {
            size_t j;
            for (j = word_start; j < i && o + 1u < out_size; ++j) {
                char c = name[j];
                out[o++] = (char)((c >= 'a' && c <= 'z') ? (c - 'a' + 'A') : c);
            }
        } else {
            size_t j;
            for (j = word_start; j < i && o + 1u < out_size; ++j) {
                char c = name[j];
                if (j == word_start && c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
                out[o++] = c;
            }
        }
    }
    out[o] = '\0';
}

static void cxta_descriptor_make_scalar_hover(const char* indicator_name,
                                              char* summary,
                                              size_t summary_size,
                                              char* indication,
                                              size_t indication_size) {
    char label[96];
    cxta_descriptor_make_label(indicator_name, label, sizeof(label));
    if (summary && summary_size > 0u) {
        snprintf(summary, summary_size, "%s indicator output.", label[0] ? label : "Scalar");
    }
    if (!indication || indication_size == 0u) return;
    snprintf(indication, indication_size,
             "Generic fallback metadata; prefer an indicator-owned plot descriptor for semantic chart behavior.");
}

static void cxta_descriptor_make_field_hover(const char* indicator_name,
                                             const char* field_name,
                                             char* summary,
                                             size_t summary_size,
                                             char* indication,
                                             size_t indication_size) {
    char indicator_label[96];
    char field_label[96];
    cxta_descriptor_make_label(indicator_name, indicator_label, sizeof(indicator_label));
    cxta_descriptor_make_label(field_name, field_label, sizeof(field_label));
    if (summary && summary_size > 0u) {
        snprintf(summary, summary_size, "%s %s output field.",
                 indicator_label[0] ? indicator_label : "Indicator",
                 field_label[0] ? field_label : "value");
    }
    if (!indication || indication_size == 0u) return;
    if (strcmp(field_name ? field_name : "", "histogram") == 0) {
        snprintf(indication, indication_size,
                 "Histogram field; sign and bar expansion show spread direction and momentum.");
    } else if (strcmp(field_name ? field_name : "", "upper") == 0 ||
               strcmp(field_name ? field_name : "", "lower") == 0 ||
               strcmp(field_name ? field_name : "", "middle") == 0) {
        snprintf(indication, indication_size,
                 "Band/channel field; use distance from price and touches or breaks for context.");
    } else if (strstr(field_name ? field_name : "", "signal") != NULL ||
               strstr(field_name ? field_name : "", "break") != NULL ||
               strstr(field_name ? field_name : "", "direction") != NULL) {
        snprintf(indication, indication_size,
                 "Signal-style field; prefer transitions and confirmations over isolated values.");
    } else {
        snprintf(indication, indication_size,
                 "Plot with related fields from the same indicator to read trend, spread, or structure.");
    }
}

static const char* cxta_descriptor_field_color_fallback(
    const cxta_field_descriptor* field) {
    if (!field || !field->name) return "#93c5fd";
    if (strcmp(field->name, "histogram") == 0) return "#a855f7";
    if (strstr(field->name, "plus") != NULL ||
        strstr(field->name, "bull") != NULL ||
        strcmp(field->name, "up") == 0 ||
        strcmp(field->name, "upper") == 0) {
        return "#22c55e";
    }
    if (strstr(field->name, "minus") != NULL ||
        strstr(field->name, "bear") != NULL ||
        strcmp(field->name, "down") == 0 ||
        strcmp(field->name, "lower") == 0) {
        return "#ef4444";
    }
    if (strcmp(field->name, "signal") == 0) return "#f97316";
    if (strcmp(field->name, "line") == 0) return "#22d3ee";
    if (strcmp(field->name, "value") == 0) return "#38bdf8";
    return "#93c5fd";
}

const cxta_scalar_plot_descriptor* cxta_builtin_plot_descriptor_find(
    const char* name) {
    size_t i;

    if (!name || name[0] == '\0') return NULL;
    for (i = 0u; i < CXTA_ARRAY_COUNT(builtin_plot_descriptors); ++i) {
        if (strcmp(builtin_plot_descriptors[i].name, name) == 0) {
            return &builtin_plot_descriptors[i].plot;
        }
    }
    return NULL;
}

bool cxta_indicator_field_auto_plot(const cxta_indicator_descriptor* descriptor,
                                    const cxta_field_descriptor* field) {
    const cxta_plot_field_descriptor* plot_field;

    if (!descriptor || !field) return false;
    plot_field = cxta_indicator_plot_field_descriptor_find(descriptor->name, field->name);
    return descriptor != NULL &&
           field != NULL &&
           (plot_field ? plot_field->auto_plot : field->auto_plot) &&
           (descriptor->flags & CXTA_INDICATOR_REPAINTING) == 0u;
}

const cxta_indicator_plot_descriptor* cxta_indicator_plot_descriptor_find(
    const char* indicator_name) {
    static cxta_scalar_plot_descriptor scalar_fallback;
    static cxta_indicator_plot_descriptor indicator_fallback;
    static char scalar_label[96];
    static char scalar_hover_summary[160];
    static char scalar_hover_indication[192];
    const cxta_indicator_descriptor* indicator_descriptor;

    if (!indicator_name || indicator_name[0] == '\0') return NULL;

    indicator_descriptor = cxta_indicator_descriptor_find(indicator_name);
    if (indicator_descriptor && indicator_descriptor->plot) {
        return indicator_descriptor->plot;
    }
    if (!indicator_descriptor || indicator_descriptor->field_count != 0u) return NULL;

    cxta_descriptor_make_label(indicator_name, scalar_label, sizeof(scalar_label));
    cxta_descriptor_make_scalar_hover(indicator_name,
                                      scalar_hover_summary,
                                      sizeof(scalar_hover_summary),
                                      scalar_hover_indication,
                                      sizeof(scalar_hover_indication));
    memset(&scalar_fallback, 0, sizeof(scalar_fallback));
    scalar_fallback.auto_plot = true;
    scalar_fallback.label = scalar_label;
    scalar_fallback.pane =
        (indicator_descriptor->default_pane && indicator_descriptor->default_pane[0] != '\0')
            ? indicator_descriptor->default_pane
            : cxta_descriptor_scalar_pane_for_name(indicator_name);
    scalar_fallback.color = "#93c5fd";
    scalar_fallback.style = "line";
    scalar_fallback.scale = scalar_fallback.pane;
    scalar_fallback.hover_summary = scalar_hover_summary;
    scalar_fallback.hover_indication = scalar_hover_indication;

    memset(&indicator_fallback, 0, sizeof(indicator_fallback));
    indicator_fallback.indicator_name = indicator_name;
    indicator_fallback.scalar = &scalar_fallback;
    return &indicator_fallback;
}

const cxta_scalar_plot_descriptor* cxta_indicator_scalar_plot_descriptor_find(
    const char* indicator_name) {
    static cxta_scalar_plot_descriptor merged;
    static char hover_summary[160];
    static char hover_indication[192];
    const cxta_indicator_plot_descriptor* descriptor =
        cxta_indicator_plot_descriptor_find(indicator_name);
    const cxta_scalar_plot_descriptor* scalar;
    const char* pane;

    if (!descriptor) return NULL;
    scalar = descriptor->scalar;
    if (!scalar) return NULL;

    merged = *scalar;
    pane = merged.pane && merged.pane[0] != '\0'
               ? merged.pane
               : cxta_descriptor_scalar_pane_for_name(indicator_name);
    if (!merged.pane || merged.pane[0] == '\0') merged.pane = pane;
    if (!merged.scale || merged.scale[0] == '\0') merged.scale = pane;
    if (!merged.color || merged.color[0] == '\0') merged.color = "#93c5fd";
    if (!merged.style || merged.style[0] == '\0') merged.style = "line";
    if (!merged.hover_summary || merged.hover_summary[0] == '\0' ||
        !merged.hover_indication || merged.hover_indication[0] == '\0') {
        cxta_descriptor_make_scalar_hover(indicator_name,
                                          hover_summary,
                                          sizeof(hover_summary),
                                          hover_indication,
                                          sizeof(hover_indication));
        if (!merged.hover_summary || merged.hover_summary[0] == '\0') {
            merged.hover_summary = hover_summary;
        }
        if (!merged.hover_indication || merged.hover_indication[0] == '\0') {
            merged.hover_indication = hover_indication;
        }
    }
    return &merged;
}

const cxta_plot_field_descriptor* cxta_indicator_plot_field_descriptor_find(
    const char* indicator_name,
    const char* field_name) {
    static cxta_plot_field_descriptor fallbacks[16];
    static char field_labels[16][96];
    static char field_hover_summaries[16][192];
    static char field_hover_indications[16][224];
    static size_t fallback_index = 0u;
    cxta_plot_field_descriptor* fallback;
    char* field_label;
    char* field_hover_summary;
    char* field_hover_indication;
    const cxta_indicator_plot_descriptor* descriptor;
    const cxta_indicator_descriptor* indicator_descriptor;
    const cxta_field_descriptor* field_descriptor = NULL;
    size_t i;

    if (!field_name || field_name[0] == '\0') return NULL;
    fallback = &fallbacks[fallback_index % CXTA_ARRAY_COUNT(fallbacks)];
    field_label = field_labels[fallback_index % CXTA_ARRAY_COUNT(field_labels)];
    field_hover_summary =
        field_hover_summaries[fallback_index % CXTA_ARRAY_COUNT(field_hover_summaries)];
    field_hover_indication =
        field_hover_indications[fallback_index % CXTA_ARRAY_COUNT(field_hover_indications)];
    fallback_index++;

    indicator_descriptor = cxta_indicator_descriptor_find(indicator_name);
    if (indicator_descriptor && indicator_descriptor->fields) {
        for (i = 0u; i < indicator_descriptor->field_count; ++i) {
            if (indicator_descriptor->fields[i].name &&
                strcmp(indicator_descriptor->fields[i].name, field_name) == 0) {
                field_descriptor = &indicator_descriptor->fields[i];
                break;
            }
        }
    }

    descriptor = cxta_indicator_plot_descriptor_find(indicator_name);
    if (descriptor && descriptor->fields) {
        for (i = 0u; i < descriptor->field_count; ++i) {
            const cxta_plot_field_descriptor* field = &descriptor->fields[i];
            if (!field->field_name) continue;
            if (strcmp(field->field_name, field_name) == 0) {
                *fallback = *field;
                cxta_descriptor_make_label(field_name, field_label, 96u);
                cxta_descriptor_make_field_hover(indicator_name,
                                                 field_name,
                                                 field_hover_summary,
                                                 192u,
                                                 field_hover_indication,
                                                 224u);
                if (!fallback->label || fallback->label[0] == '\0') fallback->label = field_label;
                if (!fallback->pane || fallback->pane[0] == '\0') {
                    fallback->pane =
                        (indicator_descriptor && indicator_descriptor->default_pane &&
                         indicator_descriptor->default_pane[0] != '\0')
                            ? indicator_descriptor->default_pane
                            : cxta_descriptor_scalar_pane_for_name(indicator_name);
                }
                if (!fallback->color || fallback->color[0] == '\0') {
                    fallback->color = cxta_descriptor_field_color_fallback(field_descriptor);
                }
                if (!fallback->style || fallback->style[0] == '\0') {
                    fallback->style = strcmp(field_name, "histogram") == 0 ? "histogram" : "line";
                }
                if (!fallback->scale || fallback->scale[0] == '\0') fallback->scale = fallback->pane;
                if (!fallback->hover_summary || fallback->hover_summary[0] == '\0') {
                    fallback->hover_summary = field_hover_summary;
                }
                if (!fallback->hover_indication || fallback->hover_indication[0] == '\0') {
                    fallback->hover_indication = field_hover_indication;
                }
                return fallback;
            }
        }
    }

    if (!field_descriptor) return NULL;

    memset(fallback, 0, sizeof(*fallback));
    fallback->field_name = field_name;
    fallback->auto_plot = field_descriptor->auto_plot;
    cxta_descriptor_make_label(field_name, field_label, 96u);
    fallback->label = field_label;
    fallback->pane =
        (indicator_descriptor->default_pane && indicator_descriptor->default_pane[0] != '\0')
            ? indicator_descriptor->default_pane
            : cxta_descriptor_scalar_pane_for_name(indicator_name);
    fallback->color = cxta_descriptor_field_color_fallback(field_descriptor);
    fallback->style = strcmp(field_name, "histogram") == 0 ? "histogram" : "line";
    fallback->scale = fallback->pane;
    if (strcmp(field_name, "histogram") == 0) {
        fallback->positive_color = "#22c55e";
        fallback->negative_color = "#ef4444";
    }
    cxta_descriptor_make_field_hover(indicator_name,
                                     field_name,
                                     field_hover_summary,
                                     192u,
                                     field_hover_indication,
                                     224u);
    fallback->hover_summary = field_hover_summary;
    fallback->hover_indication = field_hover_indication;
    return fallback;
}
