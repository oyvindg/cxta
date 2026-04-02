#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_adx(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 10.0, 8.0, 9.0, 0.0},
        {0, 0.0, 11.0, 9.0, 10.0, 0.0},
        {0, 0.0, 12.0, 10.0, 11.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    cxta_adx_state st = {1.0, 1.0, 2.0, 20.0};
    const cxta_adx_output step = cxta_adx_step(0.5, 0.3, 1.2, 14, &st);
    const cxta_adx_output out = cxta_adx(&view, 14);
    assert(isfinite(step.adx));
    assert(isfinite(out.adx));
    printf("  ✓ test_adx\n");
}
