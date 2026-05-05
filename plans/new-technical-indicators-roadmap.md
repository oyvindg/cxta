# Roadmap: nye tekniske indikatorer (cxta + cxpr + Dynasty-test)

## Mål

Utvide `cxta` med et konsistent sett indikatorer som ofte etterspørres utenfor dagens dekningsflate, med:

- ren C11-implementasjon og descriptor/bridge-metadata (`CXTA_BRIDGE_FN_SPEC_EXPR` der delvis navngiving trengs),
- registrering i `cxpr`/`cxpr-cxta-adapter` (uttrykksnavn og standardverdier),
- **røyk- eller feature-YAML** i Dynasty under `tests/configs/strategies/` når indikatoren er registrert (kopier fra malene i `strategy-yaml-templates/`).

Malene ligger som `*.yaml.example` i denne mappen slik at `cmake` **ikke** plukker dem opp før de kopieres uten `.example`.

## Global sjekkliste (gjenta for hver indikator)

- [ ] Implementer kjerne-API i `libs/cxta/src/indicators/<name>.c` + header `include/cxta/indicators/<name>.h`.
- [ ] Legg inn `cxta_<name>_descriptor`, felt-metadata og `eval`/`step` etter eksisterende mønster.
- [ ] Legg inn `cxta_<name>_params[]` og **`CXTA_BRIDGE_FN_SPEC_EXPR`** med `default_value`-strenger som matcher descriptor-fallback (se README: *Named arguments and default strings*).
- [ ] Registrer i `descriptor.c` (`kExternalDescriptors[]`, `kBridgeFnSpecs[]` ved behov).
- [ ] Eksport i `include/cxta/cxta.h` hvis offentlig overflate.
- [ ] Enhetstester i `libs/cxta/tests/indicators/<name>.test.c` (og eventuelt `ctest`).
- [ ] Verifiser `cxta_adapter_expr_param_spec_for("<expr_name>", …)` i adapter-test eller ny test.
- [ ] Dynasty: registrer indikator i bridge/host hvis egen C++-wrapper trengs (`IndicatorRegistry`, osv. — følg repo-mønster).
- [ ] Kopier tilsvarende `*.yaml.example` fra `strategy-yaml-templates/` til `tests/configs/strategies/cxta-indicators/<name>_smoke.yaml` (fjern `.example`), juster uttrykk hvis API avviker fra planen.
- [ ] Kjør `generate_strategies` / relevante `ctest`-mål grønt.

---

## 1. Stochastic Momentum Index (SMI)

**Formål:** Momentum mellom midtpunkt og rekkevidde; ofte brukt ved siden av RSI/stochastic.

| | Forslag |
|---|--------|
| **cxta-funksjon** | `cxta_stoch_momentum` (eller `cxta_smi`) |
| **cxpr / uttrykksnavn** | `stoch_momentum` (anbefalt) eller `smi` som alias senere |
| **Parametre (forslag)** | `k_period` (lookback for rekkevidde), `d_period` (glatting av SMI), `signal_period` (EMA av signal) — juster mot valgt definisjon (f.eks. William Blau-lignende variant). |
| **Multi-output** | `smi`, `signal` (evt. `histogram` = smi - signal) |

### Forslag til cxpr-syntaks

```text
stoch_momentum(20, 5, 5).smi
stoch_momentum(k_period=20, d_period=5, signal_period=5).signal
close > stoch_momentum(20, 5, 5).smi and stoch_momentum(20, 5, 5).smi > stoch_momentum(20, 5, 5).signal
```

### Sjekkliste (indikator #1)

- [ ] Kjerne + descriptor + bridge med `EXPR`-defaults
- [ ] Tester i cxta
- [ ] Adapter/metadata
- [ ] YAML: `strategy-yaml-templates/stoch_momentum_smoke.yaml.example` → `tests/configs/strategies/cxta-indicators/stoch_momentum_smoke.yaml`

---

## 2. Klinger Oscillator (KO)

**Formål:** Volumbasert oscillator (ikke Chaikin Oscillator); typisk tre linjer: KVO, signal, histogram.

| | Forslag |
|---|--------|
| **cxta-funksjon** | `cxta_klinger` |
| **cxpr / uttrykksnavn** | `klinger` |
| **Parametre (typisk)** | `fast_period`, `slow_period`, `signal_period` (f.eks. 34, 55, 13) |
| **Multi-output** | `kvo`, `signal`, `histogram` |

### Forslag til cxpr-syntaks

```text
klinger(34, 55, 13).histogram > 0
klinger(fast_period=34, slow_period=55, signal_period=13).kvo
cross_above(klinger(34,55,13).kvo, klinger(34,55,13).signal)
```

### Sjekkliste (indikator #2)

- [ ] Kjerne + descriptor + bridge med `EXPR`-defaults
- [ ] Tester
- [ ] Adapter
- [ ] YAML: `klinger_smoke.yaml.example` → `cxta-indicators/klinger_smoke.yaml`

---

## 3. Elder Ray (Bull / Bear Power)

**Formål:** Avstand mellom high/low og EMA — trend og «kraft».

| | Forslag |
|---|--------|
| **cxta-funksjon** | `cxta_elder_ray` |
| **cxpr / uttrykksnavn** | `elder_ray` |
| **Parametre** | `period` (EMA, typisk 13) |
| **Multi-output** | `bull_power`, `bear_power` |

### Forslag til cxpr-syntaks

```text
elder_ray(13).bull_power > 0 and elder_ray(13).bear_power < 0
close > ema(13) and elder_ray(13).bull_power > elder_ray(13).bull_power[1]
```

### Sjekkliste (indikator #3)

- [ ] Kjerne + descriptor + bridge (`min_args` / defaults etter EMA-lengde)
- [ ] Tester
- [ ] YAML: `elder_ray_smoke.yaml.example` → `elder_ray_smoke.yaml`

---

## 4. VWAP-bånd (avvik rundt VWAP)

**Formål:** Handelsbånd rundt session/anchored VWAP (ikke bare én linje).

| | Forslag |
|---|--------|
| **cxta-funksjon** | `cxta_vwap_bands` (bygg på eksisterende VWAP-serie eller dupliser logikk konsistent med `cxta_vwap`) |
| **cxpr / uttrykksnavn** | `vwap_bands` |
| **Parametre (forslag)** | `std_period`, `mult` (eller `num_std`), evt. `anchor` hvis dere støtter flere moduser |
| **Multi-output** | `mid`, `upper`, `lower` |

### Forslag til cxpr-syntaks

```text
close > vwap_bands(20, 2.0).lower and close < vwap_bands(20, 2.0).upper
vwap_bands(std_period=20, mult=2).upper
```

### Sjekkliste (indikator #4)

- [ ] Avklar semantikk (session vs. cumulative VWAP) mot eksisterende `vwap`/`anchored_vwap`
- [ ] Descriptor + `EXPR`
- [ ] Tester
- [ ] YAML: `vwap_bands_smoke.yaml.example`

---

## 5. OHLC-volatilitetsestimatorer (kvant / research)

**Formål:** Alternativer til ATR for historisk vol fra OHLC.

| Indikator | cxpr-navn (forslag) | Parametre | Merknad |
|-----------|---------------------|-----------|---------|
| Parkinson | `parkinson_vol` | `period` | bruker high/low |
| Garman–Klass | `garman_klass_vol` | `period` | OHLC |
| Rogers–Satchell | `rogers_satchell_vol` | `period` | OHLC |
| Yang–Zhang | `yang_zhang_vol` | `period`, evt. `overnight` flagg | mer kompleks; vurder fase 2 |

### Forslag til cxpr-syntaks

```text
parkinson_vol(20) > parkinson_vol(20)[5]
garman_klass_vol(14) < atr(14) / close
```

### Sjekkliste (indikator #5–8)

- [ ] Felles navnekonvensjon `*_vol` og dokumentert annualiseringskontrakt (daglig? per bar? sqrt scaling?)
- [ ] Egen testfil per estimator eller én `ohlc_vol.test.c` med deltester
- [ ] YAML: én samlet `ohlc_vol_smoke.yaml.example` eller én per funksjon (se maler)

---

## 6. McGinley Dynamic

**Formål:** Adaptiv glidende gjennomsnitt som følger markedet tettere enn EMA.

| | Forslag |
|---|--------|
| **cxta-funksjon** | `cxta_mcginley` |
| **cxpr / uttrykksnavn** | `mcginley` |
| **Parametre** | `period`, `k_constant` (typisk 0.6 eller 4/(period+1) avhengig av variant) |

### Forslag til cxpr-syntaks

```text
close > mcginley(20)
mcginley(period=20, k=0.6)
```

### Sjekkliste (indikator #9)

- [ ] Presiser formel i header-dokumentasjon
- [ ] `EXPR`-defaults
- [ ] YAML: `mcginley_smoke.yaml.example`

---

## 7. Pivot-varianter (Camarilla, Woodie, DeMark)

**Formål:** Paritet med andre plattformer; utvid `pivot_points`-konseptet.

| | Forslag |
|---|--------|
| **cxpr-navn** | `pivot_camarilla`, `pivot_woodie`, `pivot_demark` (eller ett `pivot_points(kind=camarilla, …)` — avvei kompleksitet) |
| **Multi-output** | nivå-felt som `r4`, `r3`, `s1`, … avhengig av variant |

### Forslag til cxpr-syntaks

```text
close > pivot_camarilla().r3
pivot_woodie().pivot
```

### Sjekkliste (indikator #10+)

- [ ] Avklar sesjon/referanse (forrige dag) mot eksisterende pivot-implementasjon
- [ ] Feltnavn i descriptor matcher YAML-felt i strategier
- [ ] YAML: `pivot_variants_smoke.yaml.example` (kan dekke flere varianter i én strategi)

---

## Fil-kart (maler)

| Mal | Beskrivelse |
|-----|-------------|
| `strategy-yaml-templates/README.md` | Slik kopieres maler til Dynasty |
| `strategy-yaml-templates/*.yaml.example` | Utkast til `tests/configs/strategies/cxta-indicators/` |

## Rekkefølge (anbefalt)

1. Elder Ray (enklest, få parametre, tydelig bruk)  
2. McGinley  
3. Klinger  
4. SMI  
5. VWAP-bånd (avhengig av VWAP-semantikk)  
6. OHLC-volestimatorer (felles dokumentasjon av skalering)  
7. Pivot-varianter (mest integrasjon mot eksisterende pivot/økter)

---

## Referanser i repo

- Bridge-policy: `libs/cxta/README.md` (*Named arguments and default strings*).
- Uttrykksstabil overflate (Dynasty): `docs/specs/yaml-expression-syntax.md`.
- Eksisterende feature-lab-mønster: `tests/configs/strategies/feature-lab/swing_anchor_vwap_feature_lab.yaml`.
