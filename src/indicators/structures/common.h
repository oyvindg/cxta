/**
 * @file common.h
 * @brief Shared helpers for structure indicator descriptors.
 */

#pragma once

#include <stddef.h>

#include <cxta/indicators/descriptor.h>

#define CXTA_STRUCT_FIELD(name_literal, type_name, member_name) \
    {(name_literal), offsetof(type_name, member_name), true}

#define CXTA_STRUCT_FIELD_HIDDEN(name_literal, type_name, member_name) \
    {(name_literal), offsetof(type_name, member_name), false}

#define CXTA_STRUCTURE_DESCRIPTOR(name_literal,             \
                                  min_arg_count,            \
                                  max_arg_count,            \
                                  primary_field,            \
                                  output_type,              \
                                  state_size_value,         \
                                  fields_array,             \
                                  eval_fn,                  \
                                  step_fn,                  \
                                  params_array)             \
    {                                                       \
        (name_literal),                                     \
        (min_arg_count),                                    \
        (max_arg_count),                                    \
        -1,                                                 \
        -1,                                                 \
        (primary_field),                                    \
        CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,      \
        sizeof(output_type),                                \
        (state_size_value),                                 \
        (fields_array),                                     \
        CXTA_ARRAY_COUNT(fields_array),                     \
        NULL,                                               \
        (eval_fn),                                          \
        NULL,                                               \
        NULL,                                               \
        NULL,                                               \
        (step_fn),                                          \
        (params_array),                                     \
        CXTA_ARRAY_COUNT(params_array),                     \
    }

#define CXTA_STRUCTURE_DESCRIPTOR_NO_PARAMS(name_literal,   \
                                            min_arg_count,  \
                                            max_arg_count,  \
                                            primary_field,  \
                                            output_type,    \
                                            state_size_value, \
                                            fields_array,   \
                                            eval_fn,        \
                                            step_fn)        \
    {                                                       \
        (name_literal),                                     \
        (min_arg_count),                                    \
        (max_arg_count),                                    \
        -1,                                                 \
        -1,                                                 \
        (primary_field),                                    \
        CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,      \
        sizeof(output_type),                                \
        (state_size_value),                                 \
        (fields_array),                                     \
        CXTA_ARRAY_COUNT(fields_array),                     \
        NULL,                                               \
        (eval_fn),                                          \
        NULL,                                               \
        NULL,                                               \
        NULL,                                               \
        (step_fn),                                          \
        NULL,                                               \
        0u,                                                 \
    }
