#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_obv(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 10.0, 1000.0},
        {0, 0.0, 0.0, 0.0, 11.0, 1500.0}, /* up → +1500 */
        {0, 0.0, 0.0, 0.0, 10.5, 800.0},  /* down → -800 */
        {0, 0.0, 0.0, 0.0, 10.5, 200.0},  /* equal → 0 */
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 4, 3);
    /* OBV = 0 + 1500 - 800 + 0 = 700 */
    assert(fabs(cxta_obv(&view) - 700.0) < 1e-9);
    printf("  ✓ test_obv\n");
}
