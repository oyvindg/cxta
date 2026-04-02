#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_wedge(void) {
    const cxta_ts_wedge_result rising = cxta_ts_wedge_from_lines(
        3.0, 114.0,
        7.0, 116.0,
        4.0, 105.0,
        8.0, 112.0,
        10.0,
        0.85);
    assert(fabs(rising.upper - 117.5) < 1e-12);
    assert(fabs(rising.lower - 115.5) < 1e-12);
    assert(rising.rising == 1.0);
    assert(rising.falling == 0.0);
    assert(rising.active == 1.0);

    const cxta_ts_wedge_result falling = cxta_ts_wedge_from_lines(
        1.0, 120.0,
        4.0, 115.0,
        3.0, 100.0,
        7.0, 96.0,
        10.0,
        0.85);
    assert(falling.rising == 0.0);
    assert(falling.falling == 1.0);
    assert(falling.active == 1.0);
    printf("  ✓ test_wedge\n");
}
