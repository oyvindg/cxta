# cxta

[![CI](https://github.com/oyvindg/cxta/actions/workflows/ci.yml/badge.svg)](https://github.com/oyvindg/cxta/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

`cxta` is a standalone C11 library for technical analysis on bar-based market data. It covers a broad indicator surface, market structure helpers, and time-series primitives — without a dependency on any expression runtime, strategy engine, or host-specific framework.

```c
#include <cxta/cxta.h>
```

## What it provides

### Indicators

**Moving averages:** SMA, EMA, WMA, DEMA, TEMA, HMA, RMA, KAMA, FRAMA, VIDYA, MAMA

**Momentum:** RSI, Stochastic, StochRSI, MACD, CCI, CMO, TSI, Williams %R, Connors RSI, RVI, KST, TRIX, PPO, Coppock, Awesome Oscillator

**Volatility:** ATR, True Range, Bollinger Bands, Keltner Channel, Std Dev, Ulcer, Choppiness, TTM Squeeze

**Volume:** OBV, MFI, CMF, Chaikin, EOM, EFI, NVI, RVOL, HVOL, Volume SMA, Volume EMA, PVO

**Trend and direction:** ADX, Supertrend, Aroon, Vortex, Ichimoku, PSAR, Schaff, Dominant Cycle

**Price and statistical:** Typical, Weighted Close, BOP, DPO, Fisher, Donchian, Price Channel, Linear Regression, Z-Score, Mass Index, Extrema, Median

**VWAP variants:** VWAP, Anchored VWAP, Session VWAP

### Market structure

Pivots, Pivot Points, Break of Structure (BOS), Fair Value Gap (FVG), Order Blocks, Liquidity, Swing Failure Pattern (SFP), Wedge

### Time-series primitives

Crossovers, smoothing, candle classification, bar patterns, rolling windows, range helpers, Fibonacci levels, trendlines, divergence, ZigZag, Volume Profile

### Descriptor metadata

A descriptor layer exposes stable metadata for each indicator — name, argument counts, output field names and offsets, primary field index — so a host application can build its own registry or expression integration without introducing a reverse dependency into `cxta`.

## API shape

Most indicators expose one or both of these forms:

- A direct view-based function for "value at current bar"
- A `_step(...)` function with explicit state for incremental updates

```c
#include <cxta/indicators/ema.h>
#include <cxta/indicators/macd.h>

cxta_ema_state ema = {0};
double ema_value = cxta_ema_step(close, 20, &ema);

cxta_macd_state macd = {0};
cxta_macd_output out = cxta_macd_step(close, 12, 26, 9, &macd);
```

The two forms make the library usable both in batch-style scans over a bar view and in incremental tick-by-tick runtimes.

The descriptor API:

```c
#include <cxta/indicators/descriptor.h>

size_t count = 0;
const cxta_indicator_descriptor* items = cxta_indicator_descriptors(&count);
const cxta_indicator_descriptor* macd = cxta_indicator_descriptor_find("macd");
```

## Build

```bash
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build -j4
```

Build without tests:

```bash
cmake -S . -B build -DCXTA_BUILD_TESTS=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build -j4
```

`compile_commands.json` is written to `build/`, which matches the repo's `.clangd` configuration.

## Test

```bash
cmake --build build --target test_cxta -j4
/usr/bin/ctest --test-dir build --output-on-failure
```

With full output including `printf` from passing tests:

```bash
cmake --build build --target test_cxta -j4
/usr/bin/ctest --test-dir build -V
```

## Layout

```text
include/cxta/
  cxta.h
  indicators/
  math/
  series/
  structure/
  ts/
src/
tests/
```

## Non-goals

`cxta` does not own expression parsing, function registration in a host runtime, timeframe resolution, or symbol routing and strategy orchestration.

## Host bridge pattern

If your host uses an expression runtime (such as `cxpr`), the bridge between `cxta` descriptors and that runtime belongs in the host — not in `cxta`. This keeps `cxta` standalone: consumers who do not need expression evaluation pull in no expression dependency, and the bridge can apply host-specific naming conventions without those leaking into the library.

### Named arguments and default strings (`CXTA_BRIDGE_FN_SPEC_EXPR`)

Some hosts rewrite **named** calls such as `indicator(foo=1, bar=2)` into positional form. When a function accepts **zero required** arguments but has **multiple optional** slots, omitting leading parameters only works if the bridge can supply a **default string per slot** (same values the indicator’s descriptor `eval` path uses when arguments are missing).

- Use **`CXTA_BRIDGE_FN_SPEC`** when there are no named-arg default slots, or only a single optional parameter (no “partial name” problem).
- Use **`CXTA_BRIDGE_FN_SPEC_EXPR`** with a parallel `cxta_expr_arg_descriptor[]` when `min_args == 0` and `param_count >= 2`. Each entry’s `default_value` must stay in sync with the corresponding `cxta_descriptor_*_arg` / `eval` fallbacks in the `.c` file.

Indicators that ship expression defaults in-tree (multi-slot optional APIs) include:

| Function | Default strings (in parameter order) |
|----------|----------------------------------------|
| `swing_anchor_vwap` | See `CXTA_STRUCT_VWAP_DEFAULT_*_STR` in `include/cxta/structure/vwap.h` |
| `ttm_squeeze` | `20`, `2`, `20`, `1.5`, `20` |
| `coppock_curve` | `10`, `14`, `11` |
| `crsi` | `3`, `2`, `100` |
| `mama` | `0.5`, `0.05` |
| `parabolic_sar` | `0.02`, `0.2` |
| `dominant_cycle_period` | `10`, `40` |

`divergence` already used `CXTA_BRIDGE_FN_SPEC_EXPR` (mixed numeric and scalar-source args). When adding a new optional multi-argument indicator, either add an `expr_args` row with defaults aligned to the eval path or document that the host must pass positional arguments only.
