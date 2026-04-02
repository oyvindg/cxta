#include <cxta/cxta.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_indicator_vwap(void) {
    const cxta_series_bar bars[] = {
        {0, 10.0, 12.0, 9.0, 11.0, 100.0},
        {0, 11.0, 13.0, 10.0, 12.0, 200.0},
        {0, 12.0, 14.0, 11.0, 13.0, 300.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    assert(fabs(cxta_vwap(&view, 0) - 12.0) < 1e-10);
    assert(fabs(cxta_vwap(&view, 2) - 12.266666666666667) < 1e-10);

    {
        cxta_vwap_state st = {0.0, 0.0};
        double value = 0.0;
        for (size_t i = 0; i < 3; ++i) {
            value = cxta_vwap_step(cxta_series_typical_price(&bars[i]), bars[i].volume, &st);
        }
        assert(fabs(value - 12.0) < 1e-10);
    }
    printf("  ✓ test_vwap\n");
}
