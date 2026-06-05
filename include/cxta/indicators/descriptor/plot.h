/**
 * @file plot.h
 * @brief Plot metadata lookup helpers for expression-facing indicators.
 */

#pragma once

#include <stdbool.h>

#include <cxta/indicators/descriptor/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Find optional GUI plot metadata for one built-in market-data series.
 * @param[in] name Stable series name such as "close" or "volume".
 * @return Matching scalar plot metadata, or `NULL` when none is defined.
 */
const cxta_scalar_plot_descriptor* cxta_builtin_plot_descriptor_find(
    const char* name);

/**
 * @brief Return whether one field should be auto-plotted by the host.
 * @param[in] descriptor Owning descriptor.
 * @param[in] field Field metadata to inspect.
 * @return `true` when the field should be auto-plotted.
 */
bool cxta_indicator_field_auto_plot(const cxta_indicator_descriptor* descriptor,
                                    const cxta_field_descriptor* field);

/**
 * @brief Find optional GUI plot metadata for one indicator.
 * @param[in] indicator_name Stable expression-facing indicator name.
 * @return Matching plot descriptor, or `NULL` when none is defined.
 */
const cxta_indicator_plot_descriptor* cxta_indicator_plot_descriptor_find(
    const char* indicator_name);

/**
 * @brief Find optional GUI plot metadata for one scalar indicator output.
 * @param[in] indicator_name Stable expression-facing indicator name.
 * @return Matching scalar plot descriptor, or `NULL` when none is defined.
 */
const cxta_scalar_plot_descriptor* cxta_indicator_scalar_plot_descriptor_find(
    const char* indicator_name);

/**
 * @brief Find optional GUI plot metadata for one indicator field.
 * @param[in] indicator_name Stable expression-facing indicator name.
 * @param[in] field_name Stable output field name.
 * @return Matching field plot descriptor, or `NULL` when none is defined.
 */
const cxta_plot_field_descriptor* cxta_indicator_plot_field_descriptor_find(
    const char* indicator_name,
    const char* field_name);

#ifdef __cplusplus
}
#endif
