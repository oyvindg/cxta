#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_bop(void) {
    const cxta_series_bar bars[] = {
        {0, 8.0, 12.0, 6.0, 10.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 1, 0);
    /* (10-8)/(12-6) = 2/6 */
    assert(fabs(cxta_bop(&view) - 2.0 / 6.0) < 1e-12);
    /* Zero range */
    const cxta_series_bar flat[] = {{0, 10.0, 10.0, 10.0, 10.0, 0.0}};
    const cxta_series_bar_view vflat = cxta_series_bar_view_make(flat, 1, 0);
    assert(fabs(cxta_bop(&vflat)) < 1e-12);
    printf("  ✓ test_bop\n");
}
