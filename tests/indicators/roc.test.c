#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_roc(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 100.0, 0.0},
        {0, 0.0, 0.0, 0.0, 110.0, 0.0},
        {0, 0.0, 0.0, 0.0, 121.0, 0.0},
    };
    /* ROC(2) at idx=2: (121/100 - 1)*100 = 21 */
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    assert(fabs(cxta_roc(&view, 2) - 21.0) < 1e-9);
    /* ROC(1) at idx=2: (121/110 - 1)*100 */
    assert(fabs(cxta_roc(&view, 1) - (121.0 / 110.0 - 1.0) * 100.0) < 1e-9);
    printf("  ✓ test_roc\n");
}
