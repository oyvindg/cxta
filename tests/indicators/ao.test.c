#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_ao(void) {
    /* Flat bars: midpoint constant → AO = 0 */
    cxta_series_bar bars[34];
    for (int i = 0; i < 34; ++i) {
        bars[i].timestamp = 0;
        bars[i].open  = 0.0;
        bars[i].high  = 12.0;
        bars[i].low   = 8.0;
        bars[i].close = 0.0;
        bars[i].volume = 0.0;
    }
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 34, 33);
    assert(fabs(cxta_ao(&view)) < 1e-9);
    printf("  ✓ test_ao\n");
}
