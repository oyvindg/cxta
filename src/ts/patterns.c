/**
 * @file patterns.c
 * @brief Scalar chart-pattern predicates.
 */

#include <cxta/ts/patterns.h>
#include <math.h>

static double cxta_ts_nonneg(double x) {
    return (x < 0.0) ? 0.0 : x;
}

static double cxta_ts_max2(double a, double b) {
    return (a > b) ? a : b;
}

static double cxta_ts_min2(double a, double b) {
    return (a < b) ? a : b;
}

static int cxta_ts_nearly_equal_rel(double a, double b, double tol) {
    const double t = cxta_ts_nonneg(tol);
    const double scale = cxta_ts_max2(fabs(a), fabs(b));
    if (scale <= 1e-12) return 1;
    return fabs(a - b) <= (scale * t);
}

double cxta_ts_pattern_double_top(double peak1,
                               double trough,
                               double peak2,
                               double close,
                               double peak_tol) {
    if (!(peak1 > trough && peak2 > trough)) return 0.0;
    if (!cxta_ts_nearly_equal_rel(peak1, peak2, peak_tol)) return 0.0;
    return (close < trough) ? 1.0 : 0.0;
}

double cxta_ts_pattern_double_bottom(double trough1,
                                  double peak,
                                  double trough2,
                                  double close,
                                  double trough_tol) {
    if (!(peak > trough1 && peak > trough2)) return 0.0;
    if (!cxta_ts_nearly_equal_rel(trough1, trough2, trough_tol)) return 0.0;
    return (close > peak) ? 1.0 : 0.0;
}

double cxta_ts_pattern_head_and_shoulders(double left_shoulder,
                                       double head,
                                       double right_shoulder,
                                       double neckline,
                                       double close,
                                       double shoulder_tol) {
    if (!(head > left_shoulder && head > right_shoulder)) return 0.0;
    if (!cxta_ts_nearly_equal_rel(left_shoulder, right_shoulder, shoulder_tol)) return 0.0;
    return (close < neckline) ? 1.0 : 0.0;
}

double cxta_ts_pattern_inverse_head_and_shoulders(double left_shoulder_low,
                                               double head_low,
                                               double right_shoulder_low,
                                               double neckline,
                                               double close,
                                               double shoulder_tol) {
    if (!(head_low < left_shoulder_low && head_low < right_shoulder_low)) return 0.0;
    if (!cxta_ts_nearly_equal_rel(left_shoulder_low, right_shoulder_low, shoulder_tol)) return 0.0;
    return (close > neckline) ? 1.0 : 0.0;
}

double cxta_ts_pattern_cup_and_handle(double left_peak,
                                   double cup_low,
                                   double right_peak,
                                   double handle_low,
                                   double close,
                                   double symmetry_tol,
                                   double handle_max_ratio) {
    const double handle_max = (handle_max_ratio < 0.05) ? 0.05 : handle_max_ratio;
    if (!(left_peak > cup_low && right_peak > cup_low)) return 0.0;
    if (!cxta_ts_nearly_equal_rel(left_peak, right_peak, symmetry_tol)) return 0.0;

    const double rim = cxta_ts_min2(left_peak, right_peak);
    const double cup_depth = rim - cup_low;
    if (cup_depth <= 0.0) return 0.0;

    const double handle_depth = right_peak - handle_low;
    if (!(handle_low > cup_low && handle_depth >= 0.0)) return 0.0;
    if (handle_depth > (cup_depth * handle_max)) return 0.0;
    return (close > right_peak) ? 1.0 : 0.0;
}

double cxta_ts_pattern_inverted_cup_and_handle(double left_low,
                                            double cup_high,
                                            double right_low,
                                            double handle_high,
                                            double close,
                                            double symmetry_tol,
                                            double handle_max_ratio) {
    const double handle_max = (handle_max_ratio < 0.05) ? 0.05 : handle_max_ratio;
    if (!(cup_high > left_low && cup_high > right_low)) return 0.0;
    if (!cxta_ts_nearly_equal_rel(left_low, right_low, symmetry_tol)) return 0.0;

    const double rim = cxta_ts_max2(left_low, right_low);
    const double cup_depth = cup_high - rim;
    if (cup_depth <= 0.0) return 0.0;

    const double handle_depth = handle_high - right_low;
    if (!(handle_high < cup_high && handle_depth >= 0.0)) return 0.0;
    if (handle_depth > (cup_depth * handle_max)) return 0.0;
    return (close < right_low) ? 1.0 : 0.0;
}

