/**
 * @file pivot_points.test.c
 * @brief Unit tests for classic pivot-point helper.
 */

#include <cxta/cxta.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>

static void test_pivot_points_previous_session_reference(void) {
    const cxta_series_bar bars[] = {
        {86400ULL + 60ULL, 100.0, 110.0, 95.0, 105.0, 1000.0},
        {86400ULL + 120ULL, 105.0, 112.0, 99.0, 108.0, 1000.0},
        {86400ULL + 180ULL, 108.0, 111.0, 97.0, 102.0, 1000.0},
        {2ULL * 86400ULL + 60ULL, 102.0, 109.0, 101.0, 106.0, 1000.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 4u, 3u);
    cxta_struct_pivot_points_result out = {0};

    assert(cxta_struct_pivot_points_compute(&view, &out) == 1);
    assert(fabs(out.pp - 103.0) < 1e-12);
    assert(fabs(out.r1 - 111.0) < 1e-12);
    assert(fabs(out.s1 - 94.0) < 1e-12);
    assert(fabs(out.r2 - 120.0) < 1e-12);
    assert(fabs(out.s2 - 86.0) < 1e-12);
    assert(fabs(out.r3 - 128.0) < 1e-12);
    assert(fabs(out.s3 - 77.0) < 1e-12);
    printf("  ✓ test_pivot_points_previous_session_reference\n");
}

static void test_pivot_points_without_previous_session_returns_zero(void) {
    const cxta_series_bar bars[] = {
        {86400ULL + 60ULL, 100.0, 110.0, 95.0, 105.0, 1000.0},
        {86400ULL + 120ULL, 105.0, 112.0, 99.0, 108.0, 1000.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 2u, 1u);
    cxta_struct_pivot_points_result out = {0};

    assert(cxta_struct_pivot_points_compute(&view, &out) == 1);
    assert(fabs(out.pp) < 1e-12);
    assert(fabs(out.r1) < 1e-12);
    assert(fabs(out.s1) < 1e-12);
    printf("  ✓ test_pivot_points_without_previous_session_returns_zero\n");
}

void cxta_test_pivot_points(void) {
    test_pivot_points_previous_session_reference();
    test_pivot_points_without_previous_session_returns_zero();
}
