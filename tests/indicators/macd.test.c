#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_macd(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 100.0, 0.0},
        {0, 0.0, 0.0, 0.0, 101.0, 0.0},
        {0, 0.0, 0.0, 0.0, 102.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    cxta_macd_state st = {100.0, 100.0, 0.0};
    const cxta_macd_output step = cxta_macd_step(101.0, 12, 26, 9, &st);
    const cxta_macd_output out = cxta_macd(&view, 12, 26, 9);
    assert(isfinite(step.line));
    assert(isfinite(step.signal));
    assert(isfinite(step.histogram));
    assert(isfinite(out.line));
    assert(isfinite(out.signal));
    assert(isfinite(out.histogram));
    printf("  ✓ test_macd\n");
}
