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
