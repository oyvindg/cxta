#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_trix(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    assert(fabs(cxta_trix(&view, 3)) < 1e-12);

    {
        cxta_trix_state st = {0.0, 0.0, 0.0, 0.0, 0.0};
        assert(fabs(cxta_trix_step(10.0, 3, &st)) < 1e-12);
        assert(fabs(cxta_trix_step(10.0, 3, &st)) < 1e-12);
    }
    printf("  ✓ test_trix\n");
}
