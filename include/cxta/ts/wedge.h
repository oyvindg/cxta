/**
 * @file wedge.h
 * @brief Wedge geometry and classification primitives.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cxta_ts_wedge_result {
    double upper;
    double lower;
    double width;
    double upper_slope;
    double lower_slope;
    double active;
    double rising;
    double falling;
} cxta_ts_wedge_result;

/**
 * @brief Compute the slope of a line segment.
 * @param[in] x1 X coordinate of the first point.
 * @param[in] y1 Y coordinate of the first point.
 * @param[in] x2 X coordinate of the second point.
 * @param[in] y2 Y coordinate of the second point.
 * @return Segment slope, or 0.0 when `x2 <= x1`.
 */
double cxta_ts_wedge_slope(double x1, double y1, double x2, double y2);

/**
 * @brief Project a line segment to an evaluation x-coordinate.
 * @param[in] x1 X coordinate of the first point.
 * @param[in] y1 Y coordinate of the first point.
 * @param[in] x2 X coordinate of the second point.
 * @param[in] y2 Y coordinate of the second point.
 * @param[in] x X coordinate to evaluate.
 * @return Projected y value, or `y2` when `x2 <= x1`.
 */
double cxta_ts_wedge_project(double x1, double y1, double x2, double y2, double x);

/**
 * @brief Classify a wedge from upper and lower guide lines.
 * @param[in] upper_x1 First upper-line x coordinate.
 * @param[in] upper_y1 First upper-line y coordinate.
 * @param[in] upper_x2 Second upper-line x coordinate.
 * @param[in] upper_y2 Second upper-line y coordinate.
 * @param[in] lower_x1 First lower-line x coordinate.
 * @param[in] lower_y1 First lower-line y coordinate.
 * @param[in] lower_x2 Second lower-line x coordinate.
 * @param[in] lower_y2 Second lower-line y coordinate.
 * @param[in] eval_x X coordinate where the wedge width is evaluated.
 * @param[in] max_width_ratio Maximum allowed end/start width ratio; non-positive values use the default.
 * @return Populated wedge classification result.
 */
cxta_ts_wedge_result cxta_ts_wedge_from_lines(double upper_x1,
                                        double upper_y1,
                                        double upper_x2,
                                        double upper_y2,
                                        double lower_x1,
                                        double lower_y1,
                                        double lower_x2,
                                        double lower_y2,
                                        double eval_x,
                                        double max_width_ratio);

#ifdef __cplusplus
}
#endif
