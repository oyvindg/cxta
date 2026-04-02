# cxta

[![CI](https://github.com/oyvindg/cxta/actions/workflows/ci.yml/badge.svg)](https://github.com/oyvindg/cxta/actions/workflows/ci.yml)

`cxta` is a standalone C11 technical-analysis library for bar-based market data.

It is intended to stay engine-agnostic:

- No dependency on any expression runtime
- No dependency on any strategy engine
- No C++ requirement
- No host-specific registry or bridge code

## Scope

The library currently includes:

- Math and time-series primitives
- Bar/window helpers
- Structure helpers such as pivots, BOS, FVG, order blocks, liquidity, SFP and anchored/session VWAP helpers
- A large indicator surface with both scalar and structured outputs
- Descriptor metadata for expression-facing hosts

The public umbrella header is:

```c
#include <cxta/cxta.h>
```

## API shape

Most indicators expose one or both of these forms:

- A direct view-based function for "value at current bar"
- A `_step(...)` function with explicit state for incremental updates

Example:

```c
#include <cxta/indicators/ema.h>
#include <cxta/indicators/macd.h>

cxta_ema_state ema = {0};
double ema_value = cxta_ema_step(close, 20, &ema);

cxta_macd_state macd = {0};
cxta_macd_output out = cxta_macd_step(close, 12, 26, 9, &macd);
```

This keeps the library usable both in batch-style scans over a bar view and in incremental runtimes.

## Descriptor metadata

`cxta` also exposes a descriptor layer in:

```c
#include <cxta/indicators/descriptor.h>
```

Descriptors provide stable metadata for host integrations:

- Indicator name
- Min/max argument counts
- Scalar-source argument ranges when supported
- Scalar vs struct capabilities
- Output field names
- Output field offsets
- Primary field index for structured outputs

This metadata is intentionally separate from any host bridge. A host application can build its own registry or expression integration from the descriptor inventory without introducing a dependency from `cxta` back into that host.

Example:

```c
#include <cxta/indicators/descriptor.h>

size_t count = 0;
const cxta_indicator_descriptor* items = cxta_indicator_descriptors(&count);
const cxta_indicator_descriptor* macd = cxta_indicator_descriptor_find("macd");
```

## Build

Standalone build:

```bash
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build -j4
```

Build without tests:

```bash
cmake -S . -B build -DCXTA_BUILD_TESTS=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build -j4
```

This writes `compile_commands.json` to `build/`, which matches the repo's `.clangd` configuration.

If you only want to refresh `compile_commands.json` for `clangd`:

```bash
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

## Test

Run the standalone test target:

```bash
cmake --build build --target test_cxta -j4
/usr/bin/ctest --test-dir build --output-on-failure
```

If you want all test output, including `printf` from passing tests:

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
  indicators/
  math/
  series/
  structure/
  ts/
tests/
  indicators/
  math/
  series/
  structure/
  ts/
```

## Non-goals

`cxta` does not try to own:

- Expression parsing
- Function registration in a host runtime
- Timeframe resolution outside the data passed in
- Symbol routing, cross-market lookups or strategy orchestration

## Host bridge pattern

If your host uses an expression runtime (such as `cxpr`), the bridge between
`cxta` descriptors and that runtime belongs in the host — not in `cxta`.

Keeping the bridge in the host preserves the standalone nature of `cxta`:
users who do not need expression evaluation do not pull in any expression
dependency, and the bridge can apply host-specific naming conventions (prefix
families, timeframe suffixes, source-aware composites) without those
conventions leaking into the library itself.
