#include <cxta/cxta.h>
#include <assert.h>
#include <stdio.h>

/*
 * cxts pattern tests:
 * - double top/bottom
 * - head-and-shoulders variants
 * - cup-and-handle variants
 */
void cxta_test_patterns(void) {
    assert(cxta_ts_pattern_double_top(110.0, 100.0, 109.0, 99.0, 0.03) == 1.0);
    assert(cxta_ts_pattern_double_top(110.0, 100.0, 106.0, 99.0, 0.03) == 0.0);

    assert(cxta_ts_pattern_double_bottom(90.0, 100.0, 91.0, 101.0, 0.03) == 1.0);
    assert(cxta_ts_pattern_double_bottom(90.0, 100.0, 95.0, 101.0, 0.03) == 0.0);

    assert(cxta_ts_pattern_head_and_shoulders(105.0, 112.0, 104.0, 100.0, 99.0, 0.05) == 1.0);
    assert(cxta_ts_pattern_head_and_shoulders(105.0, 104.0, 104.0, 100.0, 99.0, 0.05) == 0.0);

    assert(cxta_ts_pattern_inverse_head_and_shoulders(95.0, 88.0, 94.0, 100.0, 101.0, 0.05) == 1.0);
    assert(cxta_ts_pattern_inverse_head_and_shoulders(95.0, 95.0, 94.0, 100.0, 101.0, 0.05) == 0.0);

    assert(cxta_ts_pattern_cup_and_handle(110.0, 95.0, 109.0, 104.0, 111.0, 0.06, 0.5) == 1.0);
    assert(cxta_ts_pattern_cup_and_handle(110.0, 95.0, 109.0, 97.0, 111.0, 0.06, 0.5) == 0.0);

    assert(cxta_ts_pattern_inverted_cup_and_handle(90.0, 105.0, 91.0, 96.0, 89.0, 0.06, 0.5) == 1.0);
    assert(cxta_ts_pattern_inverted_cup_and_handle(90.0, 105.0, 91.0, 102.0, 89.0, 0.06, 0.5) == 0.0);

    printf("  ✓ test_chart_patterns\n");
}
