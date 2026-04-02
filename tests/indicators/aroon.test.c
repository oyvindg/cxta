#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_aroon(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 10.0, 5.0, 0.0, 0.0},
        {0, 0.0, 12.0, 4.0, 0.0, 0.0},
        {0, 0.0, 11.0, 3.0, 0.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    const cxta_aroon_output out = cxta_aroon(&view, 2);
    assert(fabs(out.up - 50.0) < 1e-12);
    assert(fabs(out.down - 100.0) < 1e-12);
    assert(fabs(out.oscillator + 50.0) < 1e-12);
    printf("  ✓ test_aroon\n");
}
