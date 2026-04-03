#include <cxta/cxta.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static const cxta_field_descriptor* cxta_find_field(const cxta_indicator_descriptor* descriptor,
                                                    const char* name) {
    size_t i;

    assert(descriptor);
    assert(name);

    for (i = 0; i < descriptor->field_count; ++i) {
        if (strcmp(descriptor->fields[i].name, name) == 0) {
            return &descriptor->fields[i];
        }
    }
    return NULL;
}

static int cxta_descriptor_is_dispatch_exception(const cxta_indicator_descriptor* descriptor) {
    assert(descriptor);
    return strcmp(descriptor->name, "divergence") == 0;
}

void cxta_test_descriptor(void) {
    size_t count = 0;
    size_t i;
    const cxta_indicator_descriptor* descriptors = cxta_indicator_descriptors(&count);
    const cxta_indicator_descriptor* sma = cxta_indicator_descriptor_find("sma");
    const cxta_indicator_descriptor* ema = cxta_indicator_descriptor_find("ema");
    const cxta_indicator_descriptor* rsi = cxta_indicator_descriptor_find("rsi");
    const cxta_indicator_descriptor* ao = cxta_indicator_descriptor_find("awesome_oscillator");
    const cxta_indicator_descriptor* bollinger = cxta_indicator_descriptor_find("bollinger");
    const cxta_indicator_descriptor* divergence = cxta_indicator_descriptor_find("divergence");
    const cxta_indicator_descriptor* eom = cxta_indicator_descriptor_find("ease_of_movement");
    const cxta_indicator_descriptor* vwap = cxta_indicator_descriptor_find("vwap");
    const cxta_indicator_descriptor* rolling_max =
        cxta_indicator_descriptor_find("rolling_max");
    const cxta_indicator_descriptor* zigzag = cxta_indicator_descriptor_find("zigzag");
    const cxta_indicator_descriptor* swing_pivots = cxta_indicator_descriptor_find("swing_pivots");
    const cxta_indicator_descriptor* structure = cxta_indicator_descriptor_find("structure");
    const cxta_indicator_descriptor* bos = cxta_indicator_descriptor_find("bos");
    const cxta_indicator_descriptor* fvg = cxta_indicator_descriptor_find("fvg");
    const cxta_indicator_descriptor* order_block = cxta_indicator_descriptor_find("order_block");
    const cxta_indicator_descriptor* liquidity = cxta_indicator_descriptor_find("liquidity");
    const cxta_indicator_descriptor* sfp = cxta_indicator_descriptor_find("sfp");
    const cxta_indicator_descriptor* pivot_points =
        cxta_indicator_descriptor_find("pivot_points");
    const cxta_indicator_descriptor* swing_anchor_vwap =
        cxta_indicator_descriptor_find("swing_anchor_vwap");
    const cxta_indicator_descriptor* wedge = cxta_indicator_descriptor_find("wedge");
    const cxta_field_descriptor* percent_b = NULL;
    const cxta_field_descriptor* vwap_value = NULL;
    const cxta_field_descriptor* pivot_points_pp = NULL;
    const cxta_field_descriptor* wedge_upper = NULL;
    const cxta_field_descriptor* wedge_breakdown = NULL;
    const cxta_series_bar bars[] = {
        {0, 0.0, 10.0, 8.0, 9.0, 100.0},
        {0, 0.0, 11.0, 9.0, 10.0, 100.0},
        {0, 0.0, 12.0, 10.0, 11.0, 200.0},
        {0, 0.0, 13.0, 11.0, 12.0, 200.0},
    };
    const cxta_series_bar structured_bars[] = {
        {0, 2.2, 1.0, 2.0, 0.8, 1000.0},
        {0, 1.7, 3.0, 1.5, 2.8, 1001.0},
        {0, 1.2, 2.0, 1.0, 1.8, 1002.0},
        {0, 1.8, 4.0, 1.6, 3.8, 1003.0},
        {0, 1.4, 3.0, 1.2, 2.8, 1004.0},
        {0, 2.0, 5.0, 1.8, 4.8, 1005.0},
        {0, 1.6, 4.0, 1.4, 3.8, 1006.0},
    };
    const cxta_series_bar pivot_point_bars[] = {
        {86400ULL + 60ULL, 100.0, 110.0, 95.0, 105.0, 1000.0},
        {86400ULL + 120ULL, 105.0, 112.0, 99.0, 108.0, 1000.0},
        {86400ULL + 180ULL, 108.0, 111.0, 97.0, 102.0, 1000.0},
        {2ULL * 86400ULL + 60ULL, 102.0, 109.0, 101.0, 106.0, 1000.0},
    };
    const cxta_series_bar fvg_bars[] = {
        {0, 9.5, 10.0, 9.0, 9.8, 1000.0},
        {0, 10.8, 11.0, 10.5, 10.9, 1000.0},
        {0, 12.2, 13.0, 12.0, 12.5, 1000.0},
        {0, 10.5, 11.2, 9.5, 10.1, 1000.0},
    };
    const cxta_series_bar savwap_bars[] = {
        {0, 9.4, 10.0, 9.0, 9.6, 100.0},
        {0, 10.0, 11.0, 9.5, 10.5, 100.0},
        {0, 9.1, 10.5, 8.5, 9.0, 100.0},
        {0, 10.8, 12.0, 9.0, 11.5, 100.0},
    };
    const double closes[] = {9.0, 10.0, 11.0, 12.0};
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 4, 3);
    const cxta_series_bar_view structured_view =
        cxta_series_bar_view_make(structured_bars, 7, 6);
    const cxta_series_bar_view pivot_points_view =
        cxta_series_bar_view_make(pivot_point_bars, 4, 3);
    const cxta_series_bar_view fvg_view = cxta_series_bar_view_make(fvg_bars, 4, 3);
    const cxta_series_bar_view savwap_view =
        cxta_series_bar_view_make(savwap_bars, 4, 3);
    const cxta_series_scalar_view close_source = cxta_series_scalar_view_make(closes, 4, 3);
    const double period2[] = {2.0};
    const double ao_args[] = {2.0, 3.0};
    const double pivot_args[] = {1.0, 1.0};
    const double fvg_args[] = {20.0};
    const double savwap_args[] = {2.0, 20.0, 0.0, 10.0};
    cxta_bollinger_output bollinger_out = {0};
    cxta_struct_pivot_args parsed_pivots = {0};
    cxta_struct_pivot_state expected_pivots = {0};
    cxta_struct_pivot_state pivot_out = {0};
    cxta_struct_pivot_points_result pivot_points_out = {0};
    cxta_struct_structure_state expected_structure = {0};
    cxta_struct_structure_state structure_out = {0};
    cxta_struct_fvg_state expected_fvg = {0};
    cxta_struct_fvg_state fvg_out = {0};
    cxta_struct_vwap_args parsed_vwap = {0};
    cxta_struct_vwap_result expected_vwap = {0};
    cxta_struct_vwap_result savwap_out = {0};
    const cxta_bollinger_output expected_bollinger = cxta_bollinger(&view, 3, 2.0);

    assert(descriptors);
    assert(count == 88u);

    for (i = 0; i < count; ++i) {
        const cxta_indicator_descriptor* descriptor = &descriptors[i];
        const unsigned is_scalar = descriptor->flags & CXTA_INDICATOR_SCALAR;
        const unsigned is_struct = descriptor->flags & CXTA_INDICATOR_STRUCT;
        const unsigned is_scalar_source = descriptor->flags & CXTA_INDICATOR_SCALAR_SOURCE;

        assert(descriptor->name);
        if (descriptor->step_scalar != NULL || descriptor->step_struct != NULL) {
            assert(descriptor->state_size > 0u || descriptor->state_slots != NULL);
        }

        if (is_struct) {
            assert(descriptor->output_size > 0u);
            if (!cxta_descriptor_is_dispatch_exception(descriptor)) {
                assert(descriptor->eval_struct != NULL);
            }
        } else {
            assert(descriptor->output_size == 0u);
        }

        if (is_scalar_source) {
            assert(descriptor->eval_scalar_src != NULL);
        }

        if (!cxta_descriptor_is_dispatch_exception(descriptor)) {
            assert(descriptor->eval_scalar != NULL || descriptor->eval_struct != NULL);
        }

        if (is_scalar && !is_struct) {
            assert(descriptor->eval_scalar != NULL);
        }
    }

    assert(sma);
    assert(sma->min_args == 1);
    assert(sma->max_args == 1);
    assert(sma->scalar_source_min_args == 1);
    assert(sma->scalar_source_max_args == 1);
    assert((sma->flags & CXTA_INDICATOR_SCALAR_SOURCE) != 0u);
    assert(sma->state_slots != NULL);
    assert(sma->step_scalar != NULL);
    assert(sma->state_slots(period2, 1u) == 5u);
    assert(fabs(sma->eval_scalar(&view, period2, 1u) - cxta_sma(&view, 2)) < 1e-12);
    assert(fabs(sma->eval_scalar_src(&close_source, period2, 1u) - cxta_sma(&view, 2)) < 1e-12);

    assert(ema);
    assert(ema->min_args == 1);
    assert(ema->max_args == 1);
    assert(ema->scalar_source_min_args == 1);
    assert(ema->scalar_source_max_args == 1);
    assert((ema->flags & CXTA_INDICATOR_SCALAR) != 0u);
    assert((ema->flags & CXTA_INDICATOR_SCALAR_SOURCE) != 0u);
    assert(ema->state_size == sizeof(cxta_ema_state));
    assert(ema->field_count == 0u);
    assert(ema->eval_scalar != NULL);
    assert(ema->eval_scalar_src != NULL);
    assert(fabs(ema->eval_scalar(&view, period2, 1u) - cxta_ema(&view, 2)) < 1e-12);
    assert(fabs(ema->eval_scalar_src(&close_source, period2, 1u) - cxta_ema(&view, 2)) < 1e-12);

    assert(rsi);
    assert(rsi->min_args == 1);
    assert(rsi->max_args == 1);
    assert(rsi->scalar_source_min_args == 1);
    assert(rsi->scalar_source_max_args == 1);
    assert((rsi->flags & CXTA_INDICATOR_SCALAR_SOURCE) != 0u);
    assert(rsi->step_scalar != NULL);
    assert(rsi->state_size > 0u);
    assert(fabs(rsi->eval_scalar(&view, period2, 1u) - cxta_rsi(&view, 2)) < 1e-12);
    assert(fabs(rsi->eval_scalar_src(&close_source, period2, 1u) - cxta_rsi(&view, 2)) < 1e-12);

    assert(ao);
    assert(ao->eval_scalar != NULL);
    assert(fabs(ao->eval_scalar(&view, ao_args, 2u) - 0.5) < 1e-12);

    assert(bollinger);
    assert(bollinger->min_args == 2);
    assert(bollinger->max_args == 2);
    assert(bollinger->primary_field_index == 2);
    assert((bollinger->flags & CXTA_INDICATOR_STRUCT) != 0u);
    assert(bollinger->output_size == sizeof(cxta_bollinger_output));
    assert(bollinger->eval_struct != NULL);
    assert(bollinger->field_count == 5u);
    percent_b = cxta_find_field(bollinger, "percentB");
    assert(percent_b);
    assert(percent_b->offset == offsetof(cxta_bollinger_output, percent_b));
    bollinger->eval_struct(&view, (const double[]){3.0, 2.0}, 2u, &bollinger_out);
    assert(fabs(bollinger_out.middle - expected_bollinger.middle) < 1e-12);
    assert(fabs(bollinger_out.upper - expected_bollinger.upper) < 1e-12);
    assert(fabs(bollinger_out.lower - expected_bollinger.lower) < 1e-12);

    assert(divergence);
    assert(divergence->min_args == 2);
    assert(divergence->max_args == 3);
    assert(divergence->primary_field_index == 0);
    assert(divergence->output_size == sizeof(cxta_divergence_output));
    assert(divergence->eval_scalar == NULL);
    assert(divergence->eval_struct == NULL);
    assert(divergence->field_count == 4u);
    assert(cxta_find_field(divergence, "bull_segment"));

    assert(eom);
    assert(eom->eval_scalar != NULL);
    assert(fabs(eom->eval_scalar(&view, period2, 1u) - 0.01) < 1e-12);

    assert(vwap);
    assert(vwap->min_args == 0);
    assert(vwap->max_args == 1);
    assert(vwap->output_size == 0u);
    assert(vwap->state_size == sizeof(cxta_vwap_state));
    assert(vwap->eval_scalar != NULL);
    assert(vwap->field_count == 1u);
    vwap_value = cxta_find_field(vwap, "value");
    assert(vwap_value);
    assert(vwap_value->offset == CXTA_FIELD_OFFSET_SCALAR);

    assert(rolling_max);
    assert(rolling_max->min_args == 1);
    assert(rolling_max->max_args == 1);
    assert(rolling_max->scalar_source_min_args == 1);
    assert(rolling_max->scalar_source_max_args == 1);
    assert((rolling_max->flags & CXTA_INDICATOR_SCALAR_SOURCE) != 0u);
    assert(rolling_max->eval_scalar != NULL);
    assert(rolling_max->eval_scalar_src != NULL);

    assert(zigzag);
    assert((zigzag->flags & CXTA_INDICATOR_REPAINTING) != 0u);
    assert(zigzag->field_count == 9u);

    assert(swing_pivots);
    assert(swing_pivots->min_args == 2);
    assert(swing_pivots->max_args == 3);
    assert(swing_pivots->primary_field_index == 0);
    assert(swing_pivots->field_count == 4u);

    assert(pivot_points);
    assert(pivot_points->min_args == 0);
    assert(pivot_points->max_args == 0);
    assert(pivot_points->primary_field_index == 0);
    assert(pivot_points->field_count == 7u);
    pivot_points_pp = cxta_find_field(pivot_points, "pp");
    assert(pivot_points_pp);
    assert(pivot_points_pp->auto_plot);

    assert(structure);
    assert(structure->min_args == 2);
    assert(structure->max_args == 3);
    assert(structure->primary_field_index == 6);
    assert(structure->field_count == 7u);

    assert(bos);
    assert(bos->min_args == 2);
    assert(bos->max_args == 3);
    assert(bos->field_count == 5u);

    assert(fvg);
    assert(fvg->min_args == 0);
    assert(fvg->max_args == 1);
    assert(fvg->field_count == 5u);

    assert(order_block);
    assert(order_block->min_args == 2);
    assert(order_block->max_args == 3);
    assert(order_block->field_count == 5u);

    assert(liquidity);
    assert(liquidity->min_args == 2);
    assert(liquidity->max_args == 4);
    assert(liquidity->field_count == 4u);

    assert(sfp);
    assert(sfp->min_args == 2);
    assert(sfp->max_args == 3);
    assert(sfp->field_count == 3u);

    assert(swing_anchor_vwap);
    assert(swing_anchor_vwap->min_args == 0);
    assert(swing_anchor_vwap->max_args == 7);
    assert(swing_anchor_vwap->primary_field_index == 0);
    assert(swing_anchor_vwap->state_size == sizeof(cxta_struct_vwap_state));
    assert(swing_anchor_vwap->field_count == 13u);
    assert(swing_anchor_vwap->step_struct != NULL);

    assert(wedge);
    assert(wedge->min_args == 2);
    assert(wedge->max_args == 4);
    assert(wedge->primary_field_index == 5);
    assert(wedge->field_count == 10u);
    wedge_upper = cxta_find_field(wedge, "upper");
    wedge_breakdown = cxta_find_field(wedge, "breakdown");
    assert(wedge_upper);
    assert(wedge_breakdown);
    assert(wedge_upper->auto_plot);
    assert(!wedge_breakdown->auto_plot);

    assert(cxta_struct_pivot_args_parse(pivot_args, 2u, structured_view.index, &parsed_pivots) == 1);
    assert(cxta_struct_pivot_state_compute(&structured_view, &parsed_pivots, &expected_pivots) == 1);
    swing_pivots->eval_struct(&structured_view, pivot_args, 2u, &pivot_out);
    assert(fabs(pivot_out.high - expected_pivots.high) < 1e-12);
    assert(fabs(pivot_out.low - expected_pivots.low) < 1e-12);
    assert(fabs(pivot_out.is_high - expected_pivots.is_high) < 1e-12);
    assert(fabs(pivot_out.is_low - expected_pivots.is_low) < 1e-12);

    pivot_points->eval_struct(&pivot_points_view, NULL, 0u, &pivot_points_out);
    assert(fabs(pivot_points_out.pp - 103.0) < 1e-12);
    assert(fabs(pivot_points_out.r1 - 111.0) < 1e-12);
    assert(fabs(pivot_points_out.s1 - 94.0) < 1e-12);

    assert(cxta_struct_structure_state_compute(&expected_pivots, &expected_structure) == 1);
    structure->eval_struct(&structured_view, pivot_args, 2u, &structure_out);
    assert(fabs(structure_out.hh - expected_structure.hh) < 1e-12);
    assert(fabs(structure_out.last_high - expected_structure.last_high) < 1e-12);
    assert(fabs(structure_out.strength - expected_structure.strength) < 1e-12);

    assert(cxta_struct_fvg_state_compute(&fvg_view, 20u, &expected_fvg) == 1);
    fvg->eval_struct(&fvg_view, fvg_args, 1u, &fvg_out);
    assert(fabs(fvg_out.bullish - expected_fvg.bullish) < 1e-12);
    assert(fabs(fvg_out.top - expected_fvg.top) < 1e-12);
    assert(fabs(fvg_out.bottom - expected_fvg.bottom) < 1e-12);
    assert(fabs(fvg_out.filled - expected_fvg.filled) < 1e-12);

    assert(cxta_struct_vwap_args_parse(savwap_args, 4u, &parsed_vwap) == 1);
    assert(cxta_struct_vwap_compute(&savwap_view, &parsed_vwap, &expected_vwap) == 1);
    swing_anchor_vwap->eval_struct(&savwap_view, savwap_args, 4u, &savwap_out);
    assert(fabs(savwap_out.value - expected_vwap.value) < 1e-12);
    assert(fabs(savwap_out.direction - expected_vwap.direction) < 1e-12);
    assert(fabs(savwap_out.reanchor - expected_vwap.reanchor) < 1e-12);
    assert(fabs(savwap_out.anchor_price - expected_vwap.anchor_price) < 1e-12);

    assert(cxta_indicator_descriptor_find(NULL) == NULL);

    printf("  ✓ test_descriptor\n");
}
