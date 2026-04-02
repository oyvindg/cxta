#include <cxta/cxta.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

void cxta_test_efi(void) {
    const cxta_series_bar bars[] = {
        {0, 0.0, 0.0, 0.0, 10.0, 0.0},
        {0, 0.0, 0.0, 0.0, 12.0, 5.0},
    };
    const cxta_series_bar_view view = cxta_series_bar_view_make(bars, 2, 1);
    assert(fabs(cxta_efi(&view, 1) - 10.0) < 1e-12);

    {
        cxta_efi_state st = {0.0, 0.0, 0.0, 0.0};
        assert(fabs(cxta_efi_step(10.0, 0.0, 1, &st)) < 1e-12);
        assert(fabs(cxta_efi_step(12.0, 5.0, 1, &st) - 10.0) < 1e-12);
    }
    printf("  ✓ test_efi\n");
}
