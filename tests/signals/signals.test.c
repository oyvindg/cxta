#include <cxta/signals/signals.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void test_basic_signals(void) {
    const double above[] = {2.0, 1.0};
    const double below[] = {1.0, 2.0};
    const double score_ascending[] = {5.0, 0.0, 10.0};
    const double score_descending[] = {5.0, 10.0, 0.0};

    assert(cxta_signal_above(above, 2) == 1.0);
    assert(cxta_signal_above(below, 2) == 0.0);
    assert(cxta_signal_below(below, 2) == 1.0);
    assert(cxta_signal_below(above, 2) == 0.0);
    assert(cxta_signal_score(score_ascending, 3) == 0.5);
    assert(cxta_signal_score(score_descending, 3) == 0.5);
    assert(cxta_signal_score(NULL, 0) == 0.0);
    printf("  ✓ test_basic_signals\n");
}

static void test_cross_signals(void) {
    const double cross_true[] = {2.0, 1.0};
    const double cross_false[] = {1.0, 2.0};
    const double cross_above[] = {3.0, 2.0, 1.0, 2.0};
    const double cross_below[] = {1.0, 2.0, 3.0, 2.0};
    const double two_arg_initial_above[] = {1.0, 2.0};
    const double two_arg_cross_above[] = {3.0, 2.0};
    const double two_arg_initial_below[] = {3.0, 2.0};
    const double two_arg_cross_below[] = {1.0, 2.0};

    assert(cxta_signal_cross(cross_true, 2) == 1.0);
    assert(cxta_signal_cross(cross_false, 2) == 0.0);
    assert(cxta_signal_cross_above(cross_above, 4) == 1.0);
    assert(cxta_signal_cross_below(cross_below, 4) == 1.0);
    assert(cxta_signal_cross_above(two_arg_initial_above, 2) == 0.0);
    assert(cxta_signal_cross_above(two_arg_cross_above, 2) == 1.0);
    assert(cxta_signal_cross_below(two_arg_initial_below, 2) == 0.0);
    assert(cxta_signal_cross_below(two_arg_cross_below, 2) == 1.0);
    assert(cxta_signal_cross_above(NULL, 0) == 0.0);
    assert(cxta_signal_cross_below(NULL, 0) == 0.0);
    printf("  ✓ test_cross_signals\n");
}

static void test_signal_pattern_descriptor_types(void) {
    size_t count = 0u;
    const cxta_signal_pattern_descriptor* descriptors =
        cxta_signal_pattern_descriptors(&count);
    int found_cross_above = 0;
    int found_score = 0;

    assert(descriptors != NULL);
    assert(count > 0u);
    for (size_t i = 0u; i < count; ++i) {
        if (strcmp(descriptors[i].name, "cross_above") == 0) {
            assert(descriptors[i].return_type == CXTA_SIGNAL_PATTERN_BOOL);
            found_cross_above = 1;
        }
        if (strcmp(descriptors[i].name, "score") == 0) {
            assert(descriptors[i].return_type == CXTA_SIGNAL_PATTERN_NUMBER);
            found_score = 1;
        }
    }

    assert(found_cross_above);
    assert(found_score);
    printf("  ✓ test_signal_pattern_descriptor_types\n");
}

void cxta_test_signals(void) {
    test_basic_signals();
    test_cross_signals();
    test_signal_pattern_descriptor_types();
}
