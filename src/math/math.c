/**
 * @file math.c
 * @brief Core scalar math helpers implementation.
 */

#include <cxta/math/math.h>
#include <math.h>
#include <stddef.h>

double cxta_math_clamp(double x, double lo, double hi) {
    if (lo > hi) {
        const double t = lo;
        lo = hi;
        hi = t;
    }
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

double cxta_math_sign(double x) {
    return (x > 0.0) ? 1.0 : ((x < 0.0) ? -1.0 : 0.0);
}

double cxta_math_cmp(double a, double b) {
    if (a > b) return 1.0;
    if (a < b) return -1.0;
    return 0.0;
}

double cxta_math_min2(double a, double b) {
    return (a < b) ? a : b;
}

double cxta_math_max2(double a, double b) {
    return (a > b) ? a : b;
}

double cxta_math_min_n(const double* a, size_t n) {
    if (!a || n == 0) return 0.0;
    double out = a[0];
    for (size_t i = 1; i < n; ++i) {
        if (a[i] < out) out = a[i];
    }
    return out;
}

double cxta_math_max_n(const double* a, size_t n) {
    if (!a || n == 0) return 0.0;
    double out = a[0];
    for (size_t i = 1; i < n; ++i) {
        if (a[i] > out) out = a[i];
    }
    return out;
}

double cxta_math_lerp(double a, double b, double t) {
    return a + (b - a) * t;
}

double cxta_math_smoothstep(double x, double e0, double e1) {
    if (e0 == e1) return (x >= e1) ? 1.0 : 0.0;
    const double t = cxta_math_clamp((x - e0) / (e1 - e0), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

double cxta_math_sigmoid(double x, double center, double steepness) {
    return 1.0 / (1.0 + exp(-steepness * (x - center)));
}

double cxta_math_pi(void) {
    return 3.14159265358979323846;
}

double cxta_math_e(void) {
    return 2.71828182845904523536;
}

double cxta_math_nan(void) {
    return NAN;
}

double cxta_math_inf(void) {
    return INFINITY;
}

double cxta_math_if(double cond, double a, double b) {
    return (cond != 0.0) ? a : b;
}
