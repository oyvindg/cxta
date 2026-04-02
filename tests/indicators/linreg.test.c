#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_linreg(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 1.0, 0.0},
        {0, 0.0, 0.0, 0.0, 2.0, 0.0},
        {0, 0.0, 0.0, 0.0, 3.0, 0.0},
        {0, 0.0, 0.0, 0.0, 4.0, 0.0},
        {0, 0.0, 0.0, 0.0, 5.0, 0.0},
    };
    /* Perfect linear series → slope = 1.0 */
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 5, 4);
    assert(fabs(cxta_linreg_slope(&view, 5) - 1.0) < 1e-9);
    /* Angle of 45 degrees */
    assert(fabs(cxta_linreg_angle(&view, 5) - 45.0) < 1e-9);
    /* Flat series → slope = 0 */
    const cxta_series_bar flat[] = {
        {0, 0.0, 0.0, 0.0, 5.0, 0.0},
        {0, 0.0, 0.0, 0.0, 5.0, 0.0},
        {0, 0.0, 0.0, 0.0, 5.0, 0.0},
    };
    const cxta_series_bar_view vflat = cxta_series_bar_view_make(flat, 3, 2);
    assert(fabs(cxta_linreg_slope(&vflat, 3)) < 1e-12);
    printf("  ✓ test_linreg\n");
}
