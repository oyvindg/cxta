#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_ppo(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    assert(fabs(cxta_ppo(&view, 2, 4)) < 1e-12);

    {
        cxta_ppo_state st = {0.0, 0.0, 0.0};
        assert(fabs(cxta_ppo_step(10.0, 2, 4, &st)) < 1e-12);
        assert(fabs(cxta_ppo_step(10.0, 2, 4, &st)) < 1e-12);
    }
    printf("  ✓ test_ppo\n");
}
