#include <cxta/math/math.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

static void test_basic(void) {
    assert(cxta_math_clamp(5.0, 0.0, 10.0) == 5.0);
    assert(cxta_math_clamp(-1.0, 0.0, 10.0) == 0.0);
    assert(cxta_math_clamp(11.0, 0.0, 10.0) == 10.0);
    assert(cxta_math_sign(-2.0) == -1.0);
    assert(cxta_math_sign(0.0) == 0.0);
    assert(cxta_math_sign(2.0) == 1.0);
    assert(cxta_math_cmp(2.0, 1.0) == 1.0);
    assert(cxta_math_cmp(1.0, 1.0) == 0.0);
    assert(cxta_math_cmp(1.0, 2.0) == -1.0);
    printf("  ✓ test_basic\n");
}

static void test_interpolation(void) {
    assert(fabs(cxta_math_lerp(0.0, 10.0, 0.5) - 5.0) < 1e-12);
    assert(fabs(cxta_math_smoothstep(0.0, 0.0, 1.0) - 0.0) < 1e-12);
    assert(fabs(cxta_math_smoothstep(1.0, 0.0, 1.0) - 1.0) < 1e-12);
    assert(fabs(cxta_math_sigmoid(0.0, 0.0, 1.0) - 0.5) < 1e-12);
    printf("  ✓ test_interpolation\n");
}

static void test_minmax(void) {
    const double v[] = {4.0, -2.0, 8.0, 0.5};
    assert(cxta_math_min2(4.0, -1.0) == -1.0);
    assert(cxta_math_max2(4.0, -1.0) == 4.0);
    assert(cxta_math_min_n(v, 4) == -2.0);
    assert(cxta_math_max_n(v, 4) == 8.0);
    printf("  ✓ test_minmax\n");
}

static void test_constants(void) {
    assert(cxta_math_pi() > 3.14 && cxta_math_pi() < 3.15);
    assert(cxta_math_e() > 2.71 && cxta_math_e() < 2.72);
    assert(isnan(cxta_math_nan()));
    assert(isinf(cxta_math_inf()));
    printf("  ✓ test_constants\n");
}

static void test_if(void) {
    assert(cxta_math_if(1.0, 2.0, 3.0) == 2.0);
    assert(cxta_math_if(0.0, 2.0, 3.0) == 3.0);
    assert(cxta_math_if(-1.0, 2.0, 3.0) == 2.0);
    printf("  ✓ test_if\n");
}

void cxta_test_math(void) {
    test_basic();
    test_interpolation();
    test_minmax();
    test_constants();
    test_if();
}
