#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_rma(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 11.0, 0.0},
        {0, 0.0, 0.0, 0.0, 12.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    assert(fabs(cxta_rma(&view, 2) - 11.25) < 1e-12);

    {
        cxta_rma_state st = {0.0, 0.0};
        assert(fabs(cxta_rma_step(10.0, 2, &st) - 10.0) < 1e-12);
        assert(fabs(cxta_rma_step(11.0, 2, &st) - 10.5) < 1e-12);
        assert(fabs(cxta_rma_step(12.0, 2, &st) - 11.25) < 1e-12);
    }
    printf("  ✓ test_rma\n");
}
