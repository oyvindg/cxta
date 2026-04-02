#include <cxta/cxta.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_mama(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    const cxta_mama_output out = cxta_mama(&view, 0.5, 0.05);
    assert(fabs(out.mama - 10.0) < 1e-12);
    assert(fabs(out.fama - 10.0) < 1e-12);
    printf("  ✓ test_mama\n");
}
