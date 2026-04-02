#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_cmo(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 12.0, 0.0},
        {0, 0.0, 0.0, 0.0, 11.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    assert(fabs(cxta_cmo(&view, 2) - (100.0 / 3.0)) < 1e-12);
    printf("  ✓ test_cmo\n");
}
