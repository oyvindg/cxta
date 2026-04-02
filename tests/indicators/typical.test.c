#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_typical(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 12.0, 8.0, 10.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 1, 0);
    /* (12+8+10)/3 = 10 */
    assert(fabs(cxta_typical(&view) - 10.0) < 1e-12);
    printf("  ✓ test_typical\n");
}
