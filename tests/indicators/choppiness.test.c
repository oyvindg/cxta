#include <cxta/cxta.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_choppiness(void) {
    const cxta_series_bar bars[] = {
        {0, 10.0, 10.0, 10.0, 10.0, 0.0},
        {0, 10.0, 10.0, 10.0, 10.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 2, 1);
    assert(fabs(cxta_choppiness(&view, 14)) < 1e-12);
    printf("  ✓ test_choppiness\n");
}
