# Strategy YAML-malier (Dynasty)

Filer her heter `*.yaml.example` slik at **ikke** `file(GLOB … tests/configs/strategies/*.yaml)` i rot-Dynasty plukker dem opp før indikatorene finnes i cxpr/cxta.

## Bruk

1. Implementer indikatoren og registrer uttrykksnavnet (se `../new-technical-indicators-roadmap.md`).
2. Opprett katalogen `tests/configs/strategies/cxta-indicators/` i Dynasty-repoet (én gang).
3. Kopier ønsket mal:

   ```bash
   cp libs/cxta/plans/strategy-yaml-templates/elder_ray_smoke.yaml.example \
      tests/configs/strategies/cxta-indicators/elder_ray_smoke.yaml
   ```

4. Kjør `cmake --build build --target generate_strategies` og fiks evt. uttrykk hvis API avviker fra planen.
5. Legg inn integrasjonstest i Dynasty om nødvendig (samme mønster som `feature-lab/`).

## Innhold

Én `.yaml.example` per hovedspor i roadmap (noen OHLC-volestimatorer er samlet i `ohlc_vol_smoke.yaml.example`).
