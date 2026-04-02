#include <cxta/cxta.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_stochastic(void) {
    const cxta_series_bar bars[] = {
        {0, 10.0, 10.0, 10.0, 10.0, 0.0},
        {0, 10.0, 10.0, 10.0, 10.0, 0.0},
        {0, 10.0, 10.0, 10.0, 10.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    const cxta_stoch_output out = cxta_stochastic(&view, 14, 3, 3);
    assert(fabs(out.k - 50.0) < 1e-12);
    assert(fabs(out.d - 50.0) < 1e-12);
    printf("  ✓ test_stochastic\n");
}
