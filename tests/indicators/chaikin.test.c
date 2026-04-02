#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_chaikin(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 10.0, 8.0, 10.0, 10.0},
        {0, 0.0, 12.0, 10.0, 12.0, 10.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 2, 1);
    assert(fabs(cxta_chaikin(&view, 1, 2) - (10.0 / 3.0)) < 1e-12);

    {
        cxta_chaikin_state st = {0.0, 0.0, 0.0, 0.0};
        assert(fabs(cxta_chaikin_step(10.0, 8.0, 10.0, 10.0, 1, 2, &st)) < 1e-12);
        assert(fabs(cxta_chaikin_step(12.0, 10.0, 12.0, 10.0, 1, 2, &st) - (10.0 / 3.0)) < 1e-12);
    }
    printf("  ✓ test_chaikin\n");
}
