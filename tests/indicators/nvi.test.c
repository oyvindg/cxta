#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_nvi(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 10.0, 100.0},
        {0, 0.0, 0.0, 0.0, 11.0, 90.0},
        {0, 0.0, 0.0, 0.0, 12.0, 100.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    assert(fabs(cxta_nvi(&view) - 1100.0) < 1e-12);
    assert(fabs(cxta_pvi(&view) - 1090.909090909091) < 1e-9);

    {
        cxta_nvi_state st_nvi = {0.0, 0.0, 0.0, 0.0};
        cxta_nvi_state st_pvi = {0.0, 0.0, 0.0, 0.0};
        assert(fabs(cxta_nvi_step(10.0, 100.0, &st_nvi) - 1000.0) < 1e-12);
        assert(fabs(cxta_nvi_step(11.0, 90.0, &st_nvi) - 1100.0) < 1e-12);
        assert(fabs(cxta_pvi_step(10.0, 100.0, &st_pvi) - 1000.0) < 1e-12);
        assert(fabs(cxta_pvi_step(11.0, 90.0, &st_pvi) - 1000.0) < 1e-12);
        assert(fabs(cxta_pvi_step(12.0, 100.0, &st_pvi) - 1090.909090909091) < 1e-9);
    }
    printf("  ✓ test_nvi\n");
}
