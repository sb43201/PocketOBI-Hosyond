# Hosyond release 1.0.0

Date: October 23, 2026. Intended Git tag: `hosyond-v1.0.0`.
Default firmware: `hosyond-cyd28`.

## Scope

Freeze of the working Hosyond/LCDWiki E32R28T port, based on upstream
TheRepairforge/PocketOBI commit e50539b (2.5.0). Upstream UI version labels remain
unchanged. The original numeric USB identification is also retained for this
release (2.4.0); it is distinct from this port's release tag.

## Changes from upstream

- Native ILI9341 driver with correct explicit-SPI DC/CS constructor order.
- GPIO23 DATA and GPIO27 ENABLE on the exposed peripheral connector.
- ESP32 GPIO output-path initialization before direct-register OneWire I/O;
  battery GPIOs reinitialized after display/touch initialization.
- SD CS held inactive; SD card/external SPI accessories excluded from this setup.
- First-run three-point calibration and BOOT-during-splash recovery shortcut.
- Calibration measured in unflipped coordinates, then selected rotation restored.
- Settings hit tests restricted to visible rows; footer/gaps no longer calibrate.
- Settings Title: back hint uses the shared right-aligned footer slot.
- Optional standalone serial diagnostics, emitted outside timed transactions and
  suppressed during PC bridge battery requests.
- Explicit demo warning for fictional data.
- PlatformIO ESP32 platform pinned to 6.12.0; normal Hosyond environment is default.

## Validation

Owner-confirmed on the target board: display, touch calibration, eight consecutive
BL1850B reads in a captured session, and battery reads through the OBI USB bridge.
Initial disconnected-pack failures were explained by the owner, not a firmware
startup problem. The final Settings boundary/alignment edits are compile-tested
and await separate panel confirmation. Repair, reset, LED commands and other
pack/board variants are not hardware-validated by this release.

Build verified for the normal release environment: ESP32 platform 6.12.0,
22,552 bytes RAM and 355,341 bytes flash. No Wi-Fi,
speaker, or additional feature work is part of this frozen release.

## License

Retains upstream PolyForm Noncommercial 1.0.0 and third-party notices. This port
is not an official upstream release. See LICENSE and THIRD-PARTY.md.
