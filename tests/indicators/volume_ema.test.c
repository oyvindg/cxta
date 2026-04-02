#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_volume_ema(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 0.0, 1000.0},
        {0, 0.0, 0.0, 0.0, 0.0, 1000.0},
        {0, 0.0, 0.0, 0.0, 0.0, 1000.0},
    };
    /* Constant volume → EMA == constant */
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    assert(fabs(cxta_volume_ema(&view, 3) - 1000.0) < 1e-9);

    cxta_volume_ema_state st = {0.0, 0.0};
    double v = 0.0;
    for (int i = 0; i < 3; ++i) v = cxta_volume_ema_step(1000.0, 3, &st);
    assert(fabs(v - cxta_volume_ema(&view, 3)) < 1e-9);
    printf("  ✓ test_volume_ema\n");
}
