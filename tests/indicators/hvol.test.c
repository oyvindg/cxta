#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_hvol(void) {
    /* Constant series → zero log returns → hvol = 0 */
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 100.0, 0.0},
        {0, 0.0, 0.0, 0.0, 100.0, 0.0},
        {0, 0.0, 0.0, 0.0, 100.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    assert(fabs(cxta_hvol(&view, 2)) < 1e-12);
    /* Non-constant series → positive volatility */
    const cxta_series_bar bars2[] = {
        {0, 0.0, 0.0, 0.0, 100.0, 0.0},
        {0, 0.0, 0.0, 0.0, 110.0, 0.0},
        {0, 0.0, 0.0, 0.0, 99.0,  0.0},
    };
    const cxta_series_bar_view view2 = cxta_series_bar_view_make(bars2, 3, 2);
    assert(cxta_hvol(&view2, 2) > 0.0);
    printf("  ✓ test_hvol\n");
}
