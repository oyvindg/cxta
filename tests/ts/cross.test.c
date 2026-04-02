#include <cxta/cxta.h>
#include <assert.h>
#include <stdio.h>

/*
 * cxts cross tests:
 * - three-way cross compare
 * - crossed above/below event detection
 */
void cxta_test_cross(void) {
    assert(cxta_ts_cross(2.0, 1.0) == 1.0);
    assert(cxta_ts_cross(1.0, 1.0) == 0.0);
    assert(cxta_ts_cross(1.0, 2.0) == -1.0);
    printf("  ✓ test_cross\n");

    assert(cxta_ts_crossed_above(1.0, 2.0, 3.0, 2.0) == 1.0);
    assert(cxta_ts_crossed_above(3.0, 2.0, 4.0, 2.0) == 0.0);
    assert(cxta_ts_crossed_above(1.0, 2.0, 1.0, 2.0) == 0.0);

    assert(cxta_ts_crossed_below(3.0, 2.0, 1.0, 2.0) == 1.0);
    assert(cxta_ts_crossed_below(1.0, 2.0, 0.0, 2.0) == 0.0);
    assert(cxta_ts_crossed_below(3.0, 2.0, 3.0, 2.0) == 0.0);
    printf("  ✓ test_cross_events\n");
}
