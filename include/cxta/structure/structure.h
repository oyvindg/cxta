/**
 * @file structure.h
 * @brief Structure classification derived from pivots.
 */

#pragma once

#include "pivot.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure snapshot derived from pivot state.
 */
typedef struct {
    double hh;
    double hl;
    double lh;
    double ll;
    double last_high;
    double last_low;
    double strength;
} cxta_struct_structure_state;

/**
 * @brief Compute structure classification from pivot state.
 */
int cxta_struct_structure_state_compute(const cxta_struct_pivot_state* pivot, cxta_struct_structure_state* out);

#ifdef __cplusplus
}
#endif
