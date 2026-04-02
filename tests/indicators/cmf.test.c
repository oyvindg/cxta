#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_cmf(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 12.0, 8.0, 12.0, 100.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 1, 0);
    assert(fabs(cxta_cmf(&view, 1) - 1.0) < 1e-12);
    printf("  ✓ test_cmf\n");
}
