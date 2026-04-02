#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_tema(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
    };
    /* Constant series → TEMA == constant */
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    assert(fabs(cxta_tema(&view, 3) - 10.0) < 1e-9);

    cxta_tema_state st = {0.0, 0.0, 0.0, 0.0};
    double v = 0.0;
    for (int i = 0; i < 3; ++i) v = cxta_tema_step(10.0, 3, &st);
    assert(fabs(v - cxta_tema(&view, 3)) < 1e-9);
    printf("  ✓ test_tema\n");
}
