/**
 * @file names.c
 * @brief Descriptor name helpers for generated function names.
 */

#include <cxta/cxta.h>

#include <stddef.h>
#include <stdio.h>

void cxta_name_sanitize_suffix(const char* name, char* out, size_t out_size) {
    size_t i;

    if (!out || out_size == 0u) return;
    out[0] = '\0';
    if (!name) return;

    for (i = 0u; name[i] != '\0' && i + 1u < out_size; ++i) {
        const char c = name[i];
        const int is_ident =
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '_';
        out[i] = is_ident ? c : '_';
    }
    out[i] = '\0';
}

int cxta_name_build_timeframe(const char* name, char* out, size_t out_size) {
    char suffix[256u];
    cxta_name_sanitize_suffix(name, suffix, sizeof(suffix));
    return snprintf(out, out_size, "%s_tf", suffix);
}
