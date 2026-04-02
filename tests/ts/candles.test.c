/**
 * @file candles.test.c
 * @brief Unit tests for cxta candlestick predicates.
 */

#include <cxta/cxta.h>
#include <assert.h>
#include <stdio.h>

void cxta_test_candles(void) {
    assert(cxta_ts_candle_bullish_engulfing(100.0, 106.0, 105.0, 101.0) == 1.0);
    assert(cxta_ts_candle_bullish_engulfing(106.0, 100.0, 105.0, 101.0) == 0.0);

    assert(cxta_ts_candle_bearish_engulfing(106.0, 100.0, 101.0, 105.0) == 1.0);
    assert(cxta_ts_candle_bearish_engulfing(100.0, 106.0, 101.0, 105.0) == 0.0);

    assert(cxta_ts_candle_hammer(100.0, 102.0, 94.0, 101.0) == 1.0);
    assert(cxta_ts_candle_hammer(101.0, 108.0, 100.0, 100.5) == 0.0);

    assert(cxta_ts_candle_shooting_star(101.0, 108.0, 100.0, 100.5) == 1.0);
    assert(cxta_ts_candle_shooting_star(100.0, 102.0, 94.0, 101.0) == 0.0);

    assert(cxta_ts_candle_doji(100.0, 105.0, 95.0, 100.6, 0.10) == 1.0);
    assert(cxta_ts_candle_doji(100.0, 105.0, 95.0, 101.7, 0.10) == 0.0);
    assert(cxta_ts_candle_doji(100.0, 105.0, 95.0, 101.7, 0.20) == 1.0);

    {
        const double args3[] = {
            100.0, 103.0, 99.0, 102.0,
            103.0, 112.0, 101.0, 104.0,
            101.5, 103.2, 97.5, 99.0,
        };
        const double args5[] = {
            100.0, 103.0, 99.0, 102.0,
            100.5, 103.5, 99.5, 101.5,
            103.0, 112.0, 101.0, 104.0,
            101.0, 103.2, 99.0, 102.0,
            101.5, 103.2, 97.5, 99.0,
        };
        const double invalid_even[] = {
            100.0, 103.0, 99.0, 102.0,
            103.0, 112.0, 101.0, 104.0,
            101.0, 103.2, 99.0, 102.0,
            101.5, 103.2, 97.5, 99.0,
        };
        assert(cxta_ts_candle_middle_finger(args3, 12u) == 1.0);
        assert(cxta_ts_candle_middle_finger(args5, 20u) == 1.0);
        assert(cxta_ts_candle_middle_finger(invalid_even, 16u) == 0.0);
    }

    printf("  ✓ test_candles\n");
}
