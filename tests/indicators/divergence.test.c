#include <cxta/cxta.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_divergence(void) {
    const double primary[] = {12.0, 10.0, 13.0, 14.0, 8.0, 13.0, 15.0};
    const double secondary[] = {12.5, 9.0, 13.0, 14.0, 9.5, 13.5, 15.0};

    {
        const cxta_series_scalar_view primary_view = cxta_series_scalar_view_make(primary, 7, 5);
        const cxta_series_scalar_view secondary_view = cxta_series_scalar_view_make(secondary, 7, 5);
        const cxta_divergence_output out =
            cxta_divergence(&primary_view, &secondary_view, 1, 1, 200);

        assert(fabs(out.bullish - 1.0) < 1e-12);
        assert(fabs(out.bearish - 0.0) < 1e-12);
        assert(isnan(out.bull_segment));
    }

    {
        const cxta_series_scalar_view primary_view = cxta_series_scalar_view_make(primary, 7, 4);
        const cxta_series_scalar_view secondary_view = cxta_series_scalar_view_make(secondary, 7, 4);
        const cxta_divergence_output out =
            cxta_divergence(&primary_view, &secondary_view, 1, 1, 200);

        assert(fabs(out.bullish - 0.0) < 1e-12);
        assert(isnan(out.bull_segment));
    }

    {
        const cxta_series_scalar_view primary_view = cxta_series_scalar_view_make(primary, 7, 5);
        const cxta_series_scalar_view secondary_view = cxta_series_scalar_view_make(secondary, 7, 5);
        const cxta_divergence_output out =
            cxta_divergence_at(&primary_view, &secondary_view, 1, 1, 200, 4);

        assert(fabs(out.bullish - 0.0) < 1e-12);
        assert(isfinite(out.bull_segment));
        assert(fabs(out.bull_segment - 9.5) < 1e-12);
    }

    printf("  ✓ test_divergence\n");
}
