#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_rsi(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 100.0, 0.0},
        {0, 0.0, 0.0, 0.0, 101.0, 0.0},
        {0, 0.0, 0.0, 0.0, 99.0, 0.0},
        {0, 0.0, 0.0, 0.0, 102.0, 0.0},
        {0, 0.0, 0.0, 0.0, 101.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 5, 4);
    cxta_rsi_state st = {0.0, 0.0};

    cxta_rsi_seed(&view, 4, &st);
    assert(fabs(st.avg_gain - 1.0) < 1e-12);
    assert(fabs(st.avg_loss - 0.75) < 1e-12);
    assert(isfinite(cxta_rsi_step(103.0, 101.0, 4, &st)));
    assert(isfinite(cxta_rsi(&view, 4)));
    printf("  ✓ test_rsi\n");
}
