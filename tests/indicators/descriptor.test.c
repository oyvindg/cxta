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
    size_t missing_plot_count = 0;
    const cxta_indicator_descriptor* descriptors = cxta_indicator_descriptors(&count);
    const cxta_indicator_descriptor* sma = cxta_indicator_descriptor_find("sma");
    const cxta_indicator_descriptor* ema = cxta_indicator_descriptor_find("ema");
    const cxta_indicator_descriptor* rsi = cxta_indicator_descriptor_find("rsi");
    const cxta_indicator_descriptor* ao = cxta_indicator_descriptor_find("awesome_oscillator");
    const cxta_indicator_descriptor* bollinger = cxta_indicator_descriptor_find("bollinger");
    const cxta_indicator_descriptor* kst = cxta_indicator_descriptor_find("kst");
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
    const cxta_indicator_descriptor* fair_value_gap =
        cxta_indicator_descriptor_find("fair_value_gap");
    const cxta_indicator_descriptor* order_block = cxta_indicator_descriptor_find("order_block");
    const cxta_indicator_descriptor* liquidity = cxta_indicator_descriptor_find("liquidity");
    const cxta_indicator_descriptor* sfp = cxta_indicator_descriptor_find("sfp");
    const cxta_indicator_descriptor* pivot_points =
        cxta_indicator_descriptor_find("pivot_points");
    const cxta_indicator_descriptor* swing_anchor_vwap =
        cxta_indicator_descriptor_find("swing_anchor_vwap");
    const cxta_indicator_descriptor* wedge = cxta_indicator_descriptor_find("wedge");
    const cxta_indicator_descriptor* broadening = cxta_indicator_descriptor_find("broadening");
    const cxta_scalar_plot_descriptor* volume_plot =
        cxta_builtin_plot_descriptor_find("volume");
    char generated_name[64] = {0};
    const cxta_field_descriptor* percent_b = NULL;
    const cxta_field_descriptor* kst_line = NULL;
    const cxta_field_descriptor* kst_histogram = NULL;
    const cxta_field_descriptor* vwap_value = NULL;
    const cxta_field_descriptor* pivot_points_pp = NULL;
    const cxta_field_descriptor* wedge_upper = NULL;
    const cxta_field_descriptor* wedge_breakdown = NULL;
    const cxta_field_descriptor* broadening_resistance = NULL;
    const cxta_field_descriptor* broadening_breakout = NULL;
    const cxta_field_descriptor* broadening_direction = NULL;
    const cxta_field_descriptor* broadening_strength = NULL;
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
    cxta_kst_output kst_out = {0};
    cxta_struct_pivot_args parsed_pivots = {0};
    cxta_struct_pivot_state expected_pivots = {0};
    cxta_struct_pivot_state pivot_out = {0};
    cxta_struct_pivot_points_result pivot_points_out = {0};
    cxta_struct_structure_state expected_structure = {0};
    cxta_struct_structure_state structure_out = {0};
    cxta_struct_fvg_state expected_fvg = {0};
    cxta_struct_fvg_state fvg_out = {0};
    cxta_fair_value_gap_output expected_fair_value_gap = {0};
    cxta_fair_value_gap_output fair_value_gap_out = {0};
    cxta_struct_vwap_args parsed_vwap = {0};
    cxta_struct_vwap_result expected_vwap = {0};
    cxta_struct_vwap_result savwap_out = {0};
    const cxta_bollinger_output expected_bollinger = cxta_bollinger(&view, 3, 2.0);

    assert(descriptors);
    assert(count == 90u);
    assert(volume_plot);
    assert(volume_plot->auto_plot);
    assert(strcmp(volume_plot->label, "Volume") == 0);
    assert(strcmp(volume_plot->pane, "price") == 0);
    assert(strcmp(volume_plot->style, "histogram") == 0);
    assert(strcmp(volume_plot->scale, "volume") == 0);
    assert(strcmp(volume_plot->positive_color, "#22c55e") == 0);
    assert(strcmp(volume_plot->negative_color, "#ef4444") == 0);

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

        if (!descriptor->plot) {
            fprintf(stderr, "missing plot descriptor: %s\n", descriptor->name);
            missing_plot_count++;
        }

        if (is_scalar && !is_struct) {
            assert(descriptor->eval_scalar != NULL);
        }

        if (descriptor->field_count == 0u) {
            const cxta_scalar_plot_descriptor* plot =
                cxta_indicator_scalar_plot_descriptor_find(descriptor->name);
            assert(plot);
            assert(plot->label && plot->label[0] != '\0');
            assert(plot->pane && plot->pane[0] != '\0');
            assert(plot->color && plot->color[0] != '\0');
            assert(plot->style && plot->style[0] != '\0');
            assert(plot->scale && plot->scale[0] != '\0');
            assert(plot->hover_summary && plot->hover_summary[0] != '\0');
            assert(plot->hover_indication && plot->hover_indication[0] != '\0');
        } else {
            size_t field_index;
            for (field_index = 0u; field_index < descriptor->field_count; ++field_index) {
                const cxta_plot_field_descriptor* plot =
                    cxta_indicator_plot_field_descriptor_find(
                        descriptor->name,
                        descriptor->fields[field_index].name);
                assert(plot);
                assert(plot->label && plot->label[0] != '\0');
                assert(plot->pane && plot->pane[0] != '\0');
                assert(plot->color && plot->color[0] != '\0');
                assert(plot->style && plot->style[0] != '\0');
                assert(plot->scale && plot->scale[0] != '\0');
                assert(plot->hover_summary && plot->hover_summary[0] != '\0');
                assert(plot->hover_indication && plot->hover_indication[0] != '\0');
            }
        }
    }
    assert(missing_plot_count == 0u);

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
    assert(cxta_indicator_descriptor_supports_scalar_source(ema));
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
    {
        const cxta_indicator_plot_descriptor* rsi_plot =
            cxta_indicator_plot_descriptor_find("rsi");
        const cxta_scalar_plot_descriptor* rsi_scalar_plot =
            cxta_indicator_scalar_plot_descriptor_find("rsi");
        assert(rsi_plot);
        assert(rsi_plot->scalar != NULL);
        assert(rsi_plot->field_count == 0u);
        assert(rsi_scalar_plot);
        assert(rsi_scalar_plot->auto_plot);
        assert(strcmp(rsi_scalar_plot->label, "RSI") == 0);
        assert(strcmp(rsi_scalar_plot->pane, "rsi") == 0);
        assert(strcmp(rsi_scalar_plot->style, "line") == 0);
        assert(strcmp(rsi_scalar_plot->scale, "rsi") == 0);
    }

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
    assert(!cxta_indicator_field_auto_plot(bollinger, percent_b));
    {
        const cxta_indicator_plot_descriptor* bollinger_plot =
            cxta_indicator_plot_descriptor_find("bollinger");
        const cxta_plot_field_descriptor* bollinger_upper =
            cxta_indicator_plot_field_descriptor_find("bollinger", "upper");
        const cxta_plot_field_descriptor* bollinger_percent_b =
            cxta_indicator_plot_field_descriptor_find("bollinger", "percentB");
        assert(bollinger_plot);
        assert(bollinger_plot->field_count == 5u);
        assert(bollinger_upper);
        assert(bollinger_upper->auto_plot);
        assert(strcmp(bollinger_upper->pane, "price") == 0);
        assert(bollinger_percent_b);
        assert(!bollinger_percent_b->auto_plot);
        assert(strcmp(bollinger_percent_b->pane, "price") == 0);
    }
    bollinger->eval_struct(&view, (const double[]){3.0, 2.0}, 2u, &bollinger_out);
    assert(fabs(bollinger_out.middle - expected_bollinger.middle) < 1e-12);
    assert(fabs(bollinger_out.upper - expected_bollinger.upper) < 1e-12);
    assert(fabs(bollinger_out.lower - expected_bollinger.lower) < 1e-12);

    assert(kst);
    assert(kst->min_args == 5);
    assert(kst->max_args == 5);
    assert(kst->primary_field_index == 0);
    assert((kst->flags & CXTA_INDICATOR_STRUCT) != 0u);
    assert(kst->output_size == sizeof(cxta_kst_output));
    assert(kst->eval_struct != NULL);
    assert(kst->field_count == 3u);
    assert(strcmp(kst->default_pane, "kst") == 0);
    kst_line = cxta_find_field(kst, "line");
    kst_histogram = cxta_find_field(kst, "histogram");
    assert(kst_line);
    assert(kst_line->offset == offsetof(cxta_kst_output, line));
    assert(kst_histogram);
    assert(kst_histogram->offset == offsetof(cxta_kst_output, histogram));
    assert(cxta_indicator_field_auto_plot(kst, kst_line));
    {
        const cxta_indicator_plot_descriptor* kst_plot =
            cxta_indicator_plot_descriptor_find("kst");
        const cxta_plot_field_descriptor* kst_signal =
            cxta_indicator_plot_field_descriptor_find("kst", "signal");
        assert(kst_plot);
        assert(kst_plot->field_count == 3u);
        assert(kst_signal);
        assert(strcmp(kst_signal->pane, "kst") == 0);
        assert(strcmp(kst_signal->style, "line") == 0);
    }
    kst->eval_struct(&view, (const double[]){2.0, 3.0, 4.0, 5.0, 3.0}, 5u, &kst_out);
    assert(fabs(kst_out.histogram - (kst_out.line - kst_out.signal)) < 1e-12);

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

    {
        const cxta_indicator_descriptor* highest =
            cxta_indicator_descriptor_find("highest");
        const cxta_indicator_descriptor* lowest =
            cxta_indicator_descriptor_find("lowest");

        assert(highest);
        assert(highest->min_args == 1);
        assert(highest->max_args == 1);
        assert(highest->scalar_source_min_args == 1);
        assert(highest->scalar_source_max_args == 1);
        assert((highest->flags & CXTA_INDICATOR_SCALAR_SOURCE) != 0u);
        assert(highest->eval_scalar != NULL);
        assert(highest->eval_scalar_src != NULL);
        assert(fabs(highest->eval_scalar(&view, period2, 1u) -
                    rolling_max->eval_scalar(&view, period2, 1u)) < 1e-12);
        assert(fabs(highest->eval_scalar_src(&close_source, period2, 1u) -
                    rolling_max->eval_scalar_src(&close_source, period2, 1u)) < 1e-12);

        assert(lowest);
        assert(lowest->min_args == 1);
        assert(lowest->max_args == 1);
        assert(lowest->scalar_source_min_args == 1);
        assert(lowest->scalar_source_max_args == 1);
        assert((lowest->flags & CXTA_INDICATOR_SCALAR_SOURCE) != 0u);
        assert(lowest->eval_scalar != NULL);
        assert(lowest->eval_scalar_src != NULL);
        {
            const cxta_indicator_descriptor* rolling_min_desc =
                cxta_indicator_descriptor_find("rolling_min");
            assert(rolling_min_desc);
            assert(fabs(lowest->eval_scalar(&view, period2, 1u) -
                        rolling_min_desc->eval_scalar(&view, period2, 1u)) < 1e-12);
            assert(fabs(lowest->eval_scalar_src(&close_source, period2, 1u) -
                        rolling_min_desc->eval_scalar_src(&close_source, period2, 1u)) < 1e-12);
        }
    }

    assert(zigzag);
    assert((zigzag->flags & CXTA_INDICATOR_REPAINTING) != 0u);
    assert(zigzag->field_count == 9u);
    {
        const cxta_indicator_plot_descriptor* zigzag_plot =
            cxta_indicator_plot_descriptor_find("zigzag");
        const cxta_plot_field_descriptor* zigzag_line =
            cxta_indicator_plot_field_descriptor_find("zigzag", "line");
        assert(zigzag_plot);
        assert(zigzag_plot->field_count == 1u);
        assert(zigzag_line);
        assert(zigzag_line->auto_plot);
        assert(strcmp(zigzag_line->pane, "zigzag") == 0);
        assert(strcmp(zigzag_line->style, "zigzag") == 0);
        assert(zigzag_line->show_price);
    }

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

    assert(fair_value_gap);
    assert(fair_value_gap->min_args == 0);
    assert(fair_value_gap->max_args == 1);
    assert(fair_value_gap->primary_field_index == 2);
    assert(fair_value_gap->field_count == 5u);

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

    assert(cxta_name_build_timeframe("macd.signal", generated_name, sizeof(generated_name)) > 0);
    assert(strcmp(generated_name, "macd_signal_tf") == 0);
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

    assert(broadening);
    assert(broadening->min_args == 0);
    assert(broadening->max_args == 10);
    assert(broadening->primary_field_index == 10);
    assert(broadening->field_count == 19u);
    broadening_resistance = cxta_find_field(broadening, "resistance");
    broadening_breakout = cxta_find_field(broadening, "breakout");
    broadening_direction = cxta_find_field(broadening, "direction");
    broadening_strength = cxta_find_field(broadening, "strength");
    assert(broadening_resistance);
    assert(broadening_breakout);
    assert(broadening_direction);
    assert(broadening_strength);
    assert(broadening_resistance->auto_plot);
    assert(!broadening_breakout->auto_plot);
    assert(!broadening_direction->auto_plot);
    assert(!broadening_strength->auto_plot);

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

    expected_fair_value_gap = cxta_fair_value_gap(&fvg_view);
    fair_value_gap->eval_struct(&fvg_view, fvg_args, 1u, &fair_value_gap_out);
    assert(fabs(fair_value_gap_out.gap_high - expected_fair_value_gap.gap_high) < 1e-12);
    assert(fabs(fair_value_gap_out.gap_low - expected_fair_value_gap.gap_low) < 1e-12);
    assert(fabs(fair_value_gap_out.direction - expected_fair_value_gap.direction) < 1e-12);
    assert(fabs(fair_value_gap_out.mitigated - expected_fair_value_gap.mitigated) < 1e-12);
    assert(fabs(fair_value_gap_out.fill_pct - expected_fair_value_gap.fill_pct) < 1e-12);

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
