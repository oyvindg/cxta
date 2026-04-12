/**
 * @file psar.c
 * @brief Parabolic SAR helpers.
 */

#include <cxta/indicators/macros.h>
#include <cxta/indicators/psar.h>
#include <string.h>

static double cxta_psar_max2(double a, double b) {
    return (a > b) ? a : b;
}

static double cxta_psar_min2(double a, double b) {
    return (a < b) ? a : b;
}

cxta_psar_output cxta_psar(const cxta_series_bar_view* view,
                           double step,
                           double max_step) {
    cxta_psar_output out = {0.0, 0.0};
    if (!view || !cxta_series_bar_view_valid(view)) return out;

    if (step < 0.0001) step = 0.0001;
    if (max_step < step) max_step = step;

    if (view->size == 1 || view->index == 0) {
        out.value = view->bars[0].close;
        return out;
    }

    {
        const size_t idx = cxta_series_clamp_index(view->size, view->index);
        int up = (view->bars[1].close >= view->bars[0].close) ? 1 : 0;
        double sar = up ? view->bars[0].low : view->bars[0].high;
        double ep = up
            ? cxta_psar_max2(view->bars[0].high, view->bars[1].high)
            : cxta_psar_min2(view->bars[0].low, view->bars[1].low);
        double af = step;

        for (size_t i = 1; i <= idx; ++i) {
            sar = sar + af * (ep - sar);

            if (up) {
                sar = cxta_psar_min2(sar, view->bars[i - 1].low);
                if (i >= 2) sar = cxta_psar_min2(sar, view->bars[i - 2].low);

                if (view->bars[i].low < sar) {
                    up = 0;
                    sar = ep;
                    ep = view->bars[i].low;
                    af = step;
                } else if (view->bars[i].high > ep) {
                    ep = view->bars[i].high;
                    af = cxta_psar_min2(max_step, af + step);
                }
            } else {
                sar = cxta_psar_max2(sar, view->bars[i - 1].high);
                if (i >= 2) sar = cxta_psar_max2(sar, view->bars[i - 2].high);

                if (view->bars[i].high > sar) {
                    up = 1;
                    sar = ep;
                    ep = view->bars[i].high;
                    af = step;
                } else if (view->bars[i].low < ep) {
                    ep = view->bars[i].low;
                    af = cxta_psar_min2(max_step, af + step);
                }
            }
        }

        out.value = sar;
        out.direction = up ? 1.0 : -1.0;
    }
    return out;
}

static const cxta_field_descriptor cxta_psar_output_fields[] = {
    CXTA_DESC_FIELD("value", offsetof(cxta_psar_output, value)),
    CXTA_DESC_FIELD("direction", offsetof(cxta_psar_output, direction)),
};

CXTA_WRAP_BAR_STRUCT_2D(cxta_parabolic_sar_desc_eval, cxta_psar_output, cxta_psar, 0.02, 0.2)

const cxta_indicator_descriptor cxta_parabolic_sar_descriptor = {
    "parabolic_sar",
    0,
    2,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_psar_output),
    sizeof(cxta_psar_state),
    cxta_psar_output_fields,
    CXTA_ARRAY_COUNT(cxta_psar_output_fields),
    NULL,
    cxta_parabolic_sar_desc_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_parabolic_sar_params,
    CXTA_ARRAY_COUNT(cxta_parabolic_sar_params),
};
