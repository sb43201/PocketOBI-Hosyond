# Hosyond Wi-Fi release 1.1.0

Date: October 25, 2026. Git tag: **hosyond-v1.1.0**.
Recommended/default build: **hosyond-cyd28-wifi**.

## Based on the original PocketOBI

This independent Hosyond E32R28T port is based on
[TheRepairforge/PocketOBI](https://github.com/TheRepairforge/PocketOBI),
commit [e50539b](https://github.com/TheRepairforge/PocketOBI/commit/e50539b)
(upstream 2.5.0). The original protocol, decoder, repair flow and most UI/assets
remain the upstream authors' work. The browser interface is adapted from
appositeit/obi-esp32; see THIRD-PARTY.md and its retained MIT notice.

## Added since Hosyond 1.0.0

- WiFiManager provisioning for router SSID/password without hardcoded credentials.
- Configurable setup AP name/key stored on the device.
- OBI-style local browser battery data/voltage, LED and confirmed error-reset controls.
- USB bridge conflict handling; browser and touch requests use one battery driver.
- A visible Settings Wi-Fi tab with SSID, IP, signal and setup details.
- Device Li-ion voltage in the header using onboard GPIO34 ADC1 and a 2:1 divider.
- Header/page-dot/tab spacing so the voltage badge stays clear of navigation.
- Five browser/API regression tests and updated public-facing setup/user guides.

The source is unchanged from the hardware-confirmed feature work; this release
promotion updates documentation, Git history and packaging only. Upstream About
and USB version identifiers remain unchanged; the Git tag identifies the port.

## Validation and limits

The owner reported the target-board touch/display, standalone battery reads,
USB PC bridge, Wi-Fi functions and unit-voltage display working. Captured
BL1850B reads were repeatable. This is not exhaustive validation across battery
models or repair/reset faults. Unit-voltage absolute accuracy and charging
behavior were not independently measured. Health percentage is an estimate.

Both firmware environments build; five browser tests pass. The current image
sizes are recorded in docs/DEVICE_POWER.md. Hardware is LCDWiki E32R28T ESP32-32E,
not the ESP32-S3 ES3C28P/ES3N28P. Final pins: **IO23 DATA / IO27 ENABLE**.

## License

The original **PolyForm Noncommercial 1.0.0** license, required notice and bundled
third-party licenses are retained. Public availability does not grant commercial
use rights beyond those licenses. See LICENSE and THIRD-PARTY.md.

---

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
