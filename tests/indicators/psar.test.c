#include <cxta/cxta.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_psar(void) {
    const cxta_series_bar bars[] = {
        {0, 9.0, 10.0, 8.0, 9.0, 100.0},
        {1, 10.0, 11.0, 9.0, 10.0, 100.0},
        {2, 11.0, 12.0, 10.0, 11.0, 100.0},
        {3, 12.0, 13.0, 11.0, 12.0, 100.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 4, 3);
    const cxta_psar_output out = cxta_psar(&view, 0.02, 0.2);

    assert(fabs(out.value - 8.16) < 1e-12);
    assert(fabs(out.direction - 1.0) < 1e-12);
    printf("  ✓ test_psar\n");
}
