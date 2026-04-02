#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_pvo(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 0.0, 100.0},
        {0, 0.0, 0.0, 0.0, 0.0, 100.0},
        {0, 0.0, 0.0, 0.0, 0.0, 100.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 3, 2);
    {
        const cxta_pvo_output out = cxta_pvo(&view, 2, 4, 3);
        assert(fabs(out.line) < 1e-12);
        assert(fabs(out.signal) < 1e-12);
        assert(fabs(out.histogram) < 1e-12);
    }

    {
        cxta_pvo_state st = {0.0, 0.0, 0.0, 0.0};
        cxta_pvo_output out = cxta_pvo_step(100.0, 2, 4, 3, &st);
        assert(fabs(out.line) < 1e-12);
        out = cxta_pvo_step(100.0, 2, 4, 3, &st);
        assert(fabs(out.line) < 1e-12);
    }
    printf("  ✓ test_pvo\n");
}
