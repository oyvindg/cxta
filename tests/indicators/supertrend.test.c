#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_supertrend(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 100.0, 99.0, 99.5, 0.0},
        {0, 0.0, 102.0, 100.0, 101.0, 0.0},
        {0, 0.0, 103.0, 101.0, 102.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    cxta_supertrend_state st = {1.0, 101.0, 99.0, 1.0};
    const cxta_supertrend_output step =
        cxta_supertrend_step(102.0, 100.0, 101.0, 100.5, 10, 3.0, &st);
    const cxta_supertrend_output out = cxta_supertrend(&view, 10, 3.0);
    assert(isfinite(step.value));
    assert(isfinite(out.value));
    printf("  ✓ test_supertrend\n");
}
