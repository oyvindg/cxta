# Descriptor Modularization Plan

## Goal

Migrate indicator metadata out of `src/indicators/descriptor.c` and into each indicator module, following the pattern now established by `macd` and `rsi`.

The end state should be:

- each indicator owns its own descriptor metadata
- each indicator exports bridge-facing named-arg metadata locally
- `src/indicators/descriptor.c` becomes a small registry and shared-helper file
- `cxpr/src/bridge/registry.c` becomes a generic bridge with minimal indicator-specific branching

## Working Pattern

Each migrated indicator should follow this shape:

1. Add local parameter metadata in `include/cxta/indicators/<name>.h`.
2. Add local bridge metadata in `include/cxta/indicators/<name>.h`.
3. Export `extern const cxta_indicator_descriptor cxta_<name>_descriptor;` from the header.
4. Define `cxta_<name>_descriptor` in `src/indicators/<name>.c`.
5. Move any indicator-specific descriptor eval wrappers into `<name>.c`.
6. Remove the inline descriptor entry from `core_descriptors[]`.
7. Add `&cxta_<name>_descriptor` to `external_descriptors[]`.
8. If the indicator should expose named args through `cxpr`, register its local `cxta_<name>_bridge_fn_spec` in `cxpr/src/bridge/registry.c`.
9. Compile-check `cxta` and `cxpr` consumers after the move.

## Per-Indicator Checklist

Use this checklist for every migration:

- [ ] Add `cxta_<name>_params[]` if the indicator exposes named parameters.
- [ ] Add `cxta_<name>_bridge_fn_spec` if the indicator should support named args in `cxpr` via `cxpr`.
- [ ] Add `extern const cxta_indicator_descriptor cxta_<name>_descriptor;` in the indicator header.
- [ ] Move field arrays into the indicator module if they are indicator-specific.
- [ ] Move eval adapters into the indicator module if they are indicator-specific.
- [ ] Move scalar-source adapters into the indicator module if present.
- [ ] Move step adapters into the indicator module if present.
- [ ] Move dynamic state-slot helpers into the indicator module if present.
- [ ] Define `cxta_<name>_descriptor` in the indicator `.c` file.
- [ ] Remove the inline descriptor entry from `core_descriptors[]`.
- [ ] Add the descriptor pointer to `external_descriptors[]`.
- [ ] Remove dead indicator-specific helper code from `descriptor.c`.
- [ ] Register the indicator through `cxpr_register_cxta_fn_spec(...)` if named args should be exposed.
- [ ] Compile-check `src/indicators/<name>.c`.
- [ ] Compile-check `src/indicators/descriptor.c`.
- [ ] Compile-check `cxpr/src/bridge/registry.c`.

## Migration Order

The order below is chosen to reduce risk and remove repetitive code first.

### Completed

- [x] `macd`
- [x] `rsi`

### Wave 1: Simple named-period indicators

These are low-risk and mostly remove repetitive parameter metadata from central files.

- [x] `ema`
- [x] `rma`
- [x] `sma`
- [x] `wma`
- [x] `atr`
- [x] `roc`
- [x] `stddev`
- [x] `zscore`
- [x] `cci`
- [x] `williams_r`
- [x] `mfi`
- [x] `dpo`
- [x] `trix`
- [x] `kama`
- [x] `frama`
- [x] `vidya`
- [x] `ulcer_index`
- [x] `rvi`

### Wave 2: Scalar-source and step-enabled indicators

These benefit more from co-locating adapters with the implementation.

- [x] `rolling_max` (includes `rolling_max_close` / `rolling_min` / `rolling_min_close` in `rolling_extrema.c`)
- [x] `rolling_min`
- [x] ~~`volume_sma`~~ / ~~`volume_ema`~~ — removed; use `sma` / `ema` with `volume` source
- [x] `ppo`
- [x] `tsi`
- [x] `elder_force`
- [x] `nvi`
- [x] `pvi`
- [x] `chaikin_oscillator`

### Wave 3: Struct-output indicators

These remove field arrays and primary-field wiring from `descriptor.c`.

- [x] `adx`
- [x] `bollinger`
- [x] `keltner`
- [x] `donchian`
- [x] `price_channel`
- [x] `stochastic`
- [x] `stoch_rsi`
- [x] `aroon`
- [x] `ichimoku`
- [x] `vortex`
- [x] `vwap`
- [x] `pvo`
- [x] `mama`
- [x] `trendline`
- [x] `ttm_squeeze`
- [x] `volume_profile`

### Wave 4: Structure-heavy and bridge-special cases

These should move last because they either have more orchestration or more bridge-specific behavior.

- [x] `pivot_points`
- [x] `swing_pivots`
- [x] `structure`
- [x] `bos`
- [x] `fvg`
- [x] `order_block`
- [x] `liquidity`
- [x] `sfp`
- [x] `wedge`
- [x] `swing_anchor_vwap`
- [x] `divergence`
- [x] `zigzag`

## Descriptor Simplification Targets

As more indicators migrate, `descriptor.c` should be reduced in stages.

### Stage 1

Keep in `descriptor.c`:

- shared numeric arg parsing
- shared period-clamp helpers
- shared struct-copy helper
- `core_descriptors[]`
- `external_descriptors[]`
- inventory lookup functions
- fallback parameter-spec table for not-yet-migrated indicators

### Stage 2

After most named-arg-capable indicators are migrated:

- remove central param-name duplication where `descriptor->params` already exists
- shrink `param_spec_table[]` to only legacy or not-yet-migrated indicators
- prefer `descriptor->params` as the source of truth

### Stage 3

After most descriptors are local:

- reduce `core_descriptors[]` to only trivial builtins or leave it empty
- keep only `external_descriptors[]` as the main registry list
- move large field arrays and adapters fully out of `descriptor.c`

## Bridge Simplification Targets

`cxpr/src/bridge/registry.c` should also simplify in stages.

### Stage 1

Current acceptable state:

- use generic `cxpr_register_cxta_fn_spec(...)`
- branch for indicators that already export local bridge specs
- fall back to generic signature-family registration for the rest

### Stage 2

After more indicators expose `cxta_bridge_fn_spec`:

- register local bridge specs for most indicator base names
- keep generic fallback only for non-migrated indicators

### Stage 3

Preferred end state:

- bridge registration is driven by local indicator metadata
- named-arg-capable indicators do not require duplicated parameter-name logic in `bridge/registry.c`
- `bridge/registry.c` becomes mostly orchestration plus generic registration helpers

## Definition of Done Per Indicator

A migration is complete when all of these are true:

- the indicator descriptor is defined in its own module
- named params, if any, are defined in the indicator header
- bridge-facing metadata, if needed, is defined in the indicator header
- `descriptor.c` no longer contains indicator-specific eval logic for that indicator
- `descriptor.c` no longer contains an inline descriptor entry for that indicator
- the descriptor is listed through `external_descriptors[]`
- `cxpr` uses local bridge metadata when named args should be exposed
- compile-checks pass for the touched `cxta` and `cxpr` files

## Next Recommended Migrations

Wave 1 single-period scalars listed above are complete. Good next targets:

1. `adx` — struct-output indicator; removes `adx_fields` and struct eval wiring from `descriptor.c`.
2. `fisher_transform` or `cmo` — remaining simple scalars still using central `CXTA_WRAP_BAR_SCALAR_1I` / `param_spec_table`.

Reasoning:

- `adx` exercises the struct-output path and is the highest-value remaining central entry in that category.
- Picking off another wrap-based scalar keeps shrinking `descriptor.c` before larger Wave 2 refactors.
