#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_ema(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 10.0, 8.0, 9.0, 0.0},
        {0, 0.0, 11.0, 9.0, 10.0, 0.0},
        {0, 0.0, 12.0, 10.0, 11.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);

    cxta_ema_state st = {0.0, 0.0};
    assert(fabs(cxta_ema_step(9.0, 2, &st) - 9.0) < 1e-12);
    assert(fabs(cxta_ema(&view, 2) - 10.555555555555555) < 1e-12);
    printf("  ✓ test_ema\n");
}
