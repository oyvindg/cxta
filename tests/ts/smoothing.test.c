#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_smoothing(void) {
    assert(cxta_ts_clamp_period(0) == 1);
    assert(cxta_ts_clamp_period(14) == 14);

    const double ema = cxta_ts_ema_step(10.0, 11.0, 10);
    assert(isfinite(ema));

    const double w = cxta_ts_wilder_step(10.0, 11.0, 10);
    assert(fabs(w - 10.1) < 1e-12);
    printf("  ✓ test_smoothing\n");
}
