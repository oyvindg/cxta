#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_dpo(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 20.0, 0.0},
        {0, 0.0, 0.0, 0.0, 30.0, 0.0},
        {0, 0.0, 0.0, 0.0, 40.0, 0.0},
        {0, 0.0, 0.0, 0.0, 50.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 5, 4);
    assert(fabs(cxta_dpo(&view, 4) + 15.0) < 1e-12);
    printf("  ✓ test_dpo\n");
}
