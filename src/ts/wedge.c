/**
 * @file wedge.c
 * @brief Wedge geometry and classification primitives.
 */

#include <cxta/ts/wedge.h>

static double cxta_ts_wedge_nonneg(double x) {
    return (x < 0.0) ? 0.0 : x;
}

double cxta_ts_wedge_slope(double x1, double y1, double x2, double y2) {
    const double dx = x2 - x1;
    if (dx <= 0.0) return 0.0;
    return (y2 - y1) / dx;
}

double cxta_ts_wedge_project(double x1, double y1, double x2, double y2, double x) {
    if (x2 <= x1) return y2;
    return y2 + cxta_ts_wedge_slope(x1, y1, x2, y2) * (x - x2);
}

static int cxta_ts_wedge_is_tight_enough(double width_start,
                                      double width_end,
                                      double max_width_ratio) {
    const double ratio = (max_width_ratio <= 0.0) ? 0.85 : max_width_ratio;
    if (!(width_start > 0.0 && width_end > 0.0)) return 0;
    if (!(width_end < width_start)) return 0;
    return width_end <= (width_start * ratio);
}

static int cxta_ts_broadening_is_wide_enough(double width_start,
                                             double width_end,
                                             double min_width_ratio) {
    const double ratio = (min_width_ratio <= 0.0) ? 1.15 : min_width_ratio;
    if (!(width_start > 0.0 && width_end > 0.0)) return 0;
    if (!(width_end > width_start)) return 0;
    return width_end >= (width_start * ratio);
}

cxta_ts_wedge_result cxta_ts_wedge_from_lines(double upper_x1,
                                        double upper_y1,
                                        double upper_x2,
                                        double upper_y2,
                                        double lower_x1,
                                        double lower_y1,
                                        double lower_x2,
                                        double lower_y2,
                                        double eval_x,
                                        double max_width_ratio) {
    cxta_ts_wedge_result out;
    out.upper_slope = cxta_ts_wedge_slope(upper_x1, upper_y1, upper_x2, upper_y2);
    out.lower_slope = cxta_ts_wedge_slope(lower_x1, lower_y1, lower_x2, lower_y2);
    out.upper = cxta_ts_wedge_project(upper_x1, upper_y1, upper_x2, upper_y2, eval_x);
    out.lower = cxta_ts_wedge_project(lower_x1, lower_y1, lower_x2, lower_y2, eval_x);
    out.width = cxta_ts_wedge_nonneg(out.upper - out.lower);
    out.active = 0.0;
    out.rising = 0.0;
    out.falling = 0.0;

    {
        const double ref_x = (upper_x1 < lower_x1) ? upper_x1 : lower_x1;
        const double upper_ref = cxta_ts_wedge_project(upper_x1, upper_y1, upper_x2, upper_y2, ref_x);
        const double lower_ref = cxta_ts_wedge_project(lower_x1, lower_y1, lower_x2, lower_y2, ref_x);
        const double width_ref = upper_ref - lower_ref;
        const int rising =
            out.upper_slope > 0.0 &&
            out.lower_slope > 0.0 &&
            out.lower_slope > out.upper_slope &&
            cxta_ts_wedge_is_tight_enough(width_ref, out.width, max_width_ratio);
        const int falling =
            out.upper_slope < 0.0 &&
            out.lower_slope < 0.0 &&
            out.upper_slope < out.lower_slope &&
            cxta_ts_wedge_is_tight_enough(width_ref, out.width, max_width_ratio);

        out.rising = rising ? 1.0 : 0.0;
        out.falling = falling ? 1.0 : 0.0;
        out.active = (rising || falling) ? 1.0 : 0.0;
    }

    return out;
}

cxta_ts_broadening_result cxta_ts_broadening_from_lines(double resistance_x1,
                                                        double resistance_y1,
                                                        double resistance_x2,
                                                        double resistance_y2,
                                                        double support_x1,
                                                        double support_y1,
                                                        double support_x2,
                                                        double support_y2,
                                                        double eval_x,
                                                        double min_width_ratio) {
    cxta_ts_broadening_result out;
    out.resistance_slope =
        cxta_ts_wedge_slope(resistance_x1, resistance_y1, resistance_x2, resistance_y2);
    out.support_slope =
        cxta_ts_wedge_slope(support_x1, support_y1, support_x2, support_y2);
    out.resistance =
        cxta_ts_wedge_project(resistance_x1, resistance_y1, resistance_x2, resistance_y2, eval_x);
    out.support =
        cxta_ts_wedge_project(support_x1, support_y1, support_x2, support_y2, eval_x);
    out.width = cxta_ts_wedge_nonneg(out.resistance - out.support);
    out.active = 0.0;
    out.rising = 0.0;
    out.falling = 0.0;

    {
        const double ref_x = (resistance_x1 < support_x1) ? resistance_x1 : support_x1;
        const double resistance_ref =
            cxta_ts_wedge_project(resistance_x1, resistance_y1, resistance_x2, resistance_y2, ref_x);
        const double support_ref =
            cxta_ts_wedge_project(support_x1, support_y1, support_x2, support_y2, ref_x);
        const double width_ref = resistance_ref - support_ref;
        const int rising =
            out.resistance_slope > 0.0 &&
            out.support_slope > 0.0 &&
            out.resistance_slope > out.support_slope &&
            cxta_ts_broadening_is_wide_enough(width_ref, out.width, min_width_ratio);
        const int falling =
            out.resistance_slope < 0.0 &&
            out.support_slope < 0.0 &&
            out.support_slope < out.resistance_slope &&
            cxta_ts_broadening_is_wide_enough(width_ref, out.width, min_width_ratio);
        const int symmetric =
            out.resistance_slope > 0.0 &&
            out.support_slope < 0.0 &&
            cxta_ts_broadening_is_wide_enough(width_ref, out.width, min_width_ratio);

        out.rising = rising ? 1.0 : 0.0;
        out.falling = falling ? 1.0 : 0.0;
        out.active = (rising || falling || symmetric) ? 1.0 : 0.0;
    }

    return out;
}
