/**
 * @file fib.test.c
 * @brief Unit tests for cxta Fibonacci predicates.
 *
 * Tests covered:
 * - fib_level: boundary ratios, midpoint, edge (high == low)
 * - fib_near: exact hit, within tolerance, outside tolerance, edge
 * - fib_zone: inside zone, boundary values, outside zone, inverted ratios
 * - fib_extension: full and partial retracement, inverted swing edge
 */

#include <cxta/cxta.h>
#include <assert.h>
#include <stdio.h>

static int fib_approx(double a, double b) {
    double diff = a - b;
    return (diff < 0.0 ? -diff : diff) < 1e-9;
}

void cxta_test_fib(void) {
    /* fib_level */
    assert(fib_approx(cxta_ts_fib_level(100.0, 80.0, 0.0),   100.0));  /* ratio 0 → high      */
    assert(fib_approx(cxta_ts_fib_level(100.0, 80.0, 1.0),    80.0));  /* ratio 1 → low       */
    assert(fib_approx(cxta_ts_fib_level(100.0, 80.0, 0.5),    90.0));  /* 50% midpoint        */
    assert(fib_approx(cxta_ts_fib_level(100.0, 80.0, 0.618),  87.64)); /* 61.8% golden        */
    assert(fib_approx(cxta_ts_fib_level(100.0, 100.0, 0.5),    0.0));  /* edge: high == low   */

    /* fib_near */
    assert(cxta_ts_fib_near(100.0, 80.0, 90.0, 0.5, 0.01) == 1.0);    /* exact hit            */
    assert(cxta_ts_fib_near(100.0, 80.0, 89.9, 0.5, 0.01) == 1.0);    /* 0.1/20=0.005 < 0.01 */
    assert(cxta_ts_fib_near(100.0, 80.0, 89.0, 0.5, 0.01) == 0.0);    /* 1.0/20=0.05  > 0.01 */
    assert(cxta_ts_fib_near(100.0, 100.0, 90.0, 0.5, 0.02) == 0.0);   /* edge: high == low    */

    /* fib_zone */
    assert(cxta_ts_fib_zone(100.0, 80.0, 90.0,  0.382, 0.618) == 1.0); /* inside golden zone  */
    assert(cxta_ts_fib_zone(100.0, 80.0, 87.64, 0.382, 0.618) == 1.0); /* at lower bound      */
    assert(cxta_ts_fib_zone(100.0, 80.0, 92.36, 0.382, 0.618) == 1.0); /* at upper bound      */
    assert(cxta_ts_fib_zone(100.0, 80.0, 78.0,  0.382, 0.618) == 0.0); /* below zone          */
    assert(cxta_ts_fib_zone(100.0, 80.0, 95.0,  0.382, 0.618) == 0.0); /* above zone          */
    assert(cxta_ts_fib_zone(100.0, 80.0, 90.0,  0.618, 0.382) == 0.0); /* edge: inverted      */

    /* fib_extension */
    assert(fib_approx(cxta_ts_fib_extension(80.0, 100.0, 80.0, 1.0),    100.0)); /* ratio 1.0, retrace == swing_low → swing_high */
    assert(fib_approx(cxta_ts_fib_extension(80.0, 100.0, 80.0, 1.618),  112.36));
    assert(fib_approx(cxta_ts_fib_extension(80.0, 100.0, 90.0, 1.618),  122.36)); /* partial retracement */
    assert(fib_approx(cxta_ts_fib_extension(100.0, 80.0, 90.0, 1.618),    0.0)); /* edge: swing_high <= swing_low */

    printf("  ✓ test_fib\n");
}
