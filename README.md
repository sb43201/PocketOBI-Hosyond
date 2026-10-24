> **Wi-Fi development branch:** build **hosyond-cyd28-wifi** (this branch's default).
> Includes WiFiManager provisioning, configurable setup AP name/key, browser battery
> controls and a Settings Wi-Fi information tab. See [Wi-Fi setup guide](docs/WIFI.md).
> Hardware testing of the new Wi-Fi functions is pending. The original offline
> release remains tagged [hosyond-v1.0.0](https://github.com/sb43201/PocketOBI-Hosyond/releases/tag/hosyond-v1.0.0).

# PocketOBI for Hosyond 2.8-inch ESP32

**Hosyond release 1.0.0 — October 23, 2026**

Standalone Makita LXT battery reader for the **Hosyond/LCDWiki E32R28T
ESP32-32E** display board. Includes a calibrated touchscreen and a USB PC bridge.
Based on [TheRepairforge/PocketOBI](https://github.com/TheRepairforge/PocketOBI),
upstream commit `e50539b` (2.5.0). This is a hardware port, not an official
TheRepairForge release.

[User manual](USER_MANUAL.md) · [Release notes](RELEASE_NOTES.md) ·
[Original upstream README](docs/UPSTREAM_README.md) · [License](LICENSE)

## Working configuration

| Item | Configuration |
| --- | --- |
| Board | LCDWiki E32R28T / ESP32-WROOM-32E, 4 MB flash |
| Display | ILI9341V, 320 x 240 landscape, backlight GPIO21 |
| Touch | XPT2046 resistive touchscreen, saved three-point calibration |
| Battery DATA | **GPIO23**, exposed SPI MOSI pin |
| Battery ENABLE | **GPIO27**, exposed SPI CS pin |
| Firmware environment | **hosyond-cyd28** (default) |
| Wi-Fi / speaker | Not used |

This is **not** the ESP32-S3 ES3C28P/ES3N28P board. Generic older CYD CN1/P3
wiring does not describe this board. Use the
[LCDWiki pin assignment](https://www.lcdwiki.com/2.8inch_ESP32-32E_Display#ESP32_Pin_Assignment).

## Wiring

Disconnect USB and the pack before changing wiring. Power the board through USB.
**Never connect Makita B+ to the board or to its single-cell battery connector.**

| Board | Battery adapter |
| --- | --- |
| IO23 / SPI MOSI | DATA — pin 6 on the owner's tested adapter |
| IO27 / SPI CS | ENABLE — pin 2 on the owner's tested adapter |
| GND | Main battery B- |
| 3V3 | Supply for DATA and ENABLE pull-up resistors |

Fit one pull-up resistor from DATA to 3V3 and another from ENABLE to 3V3.
Upstream specifies **470 ohms**; 4.7 kilohms may work with short wiring but can
be marginal. The owner's final installed resistor values were not independently
measured. Identify contacts by function: adapter numbering can differ.

```text
3V3 ----[pull-up]----+---- IO23
                    +------------ battery DATA
3V3 ----[pull-up]----+---- IO27
                    +------------ battery ENABLE
GND ----------------------------- battery B-
USB-C <--- USB power               battery B+ : NOT CONNECTED
```

Remove the microSD card and disconnect external SPI accessories. IO23 is shared
with the SD slot; firmware holds SD CS (IO5) high. Obtain 3V3 and GND from the
board's power/expansion connections. No external display wiring is needed.

## Build and upload

Install PlatformIO, open this repository folder, and select `hosyond-cyd28`.

```powershell
pio run -e hosyond-cyd28
pio run -e hosyond-cyd28 -t upload --upload-port COM8
```

Replace COM8 with the board's current port. Close Serial Monitor and browser
serial connections before uploading. PlatformIO flashes all required images at
the correct offsets; do not flash an application-only `firmware.bin` at 0x0.

On first startup after the calibration update, tap each of the three crosshairs
and release between taps. Settings are retained across ordinary uploads.

## Use

- Tap **Battery** to read the pack. Tap left/right edges to change pages; tap the
  centre to refresh. Tap the title bar to go back.
- **Tools > PC bridge** opens the USB bridge. Keep it ACTIVE, close Serial Monitor,
  then connect from the OBI browser tool. This workflow was confirmed by the owner.
- In Tools, tap the band below the three rows to change tool pages.
- **Tools > Settings** includes screen flip, bridge at startup, language, and touch
  calibration. Only visible Settings rows act; footer/gap taps do nothing.

Read the [manual](USER_MANUAL.md) before using repair/reset functions.

## Build variants

| Environment | Purpose |
| --- | --- |
| **hosyond-cyd28** | Normal release; use this for daily operation and USB bridge |
| hosyond-cyd28-diagnostic | Same hardware with battery logs at 115200; transaction logs are muted in PC bridge |
| hosyond-cyd28-demo | Fictional battery screen tour; does not read or repair a real battery |

Other upstream environments remain in platformio.ini but are not covered by this
Hosyond release's hardware validation.

## Validation and limits

The owner confirmed display operation, successful touch calibration, repeated
BL1850B reads, and USB PC bridge battery reads. Example pack readings were around
20 V with 1-2 mV cell spread. Initial all-FF reads in that captured test occurred
before the battery was connected.

The final Settings touch-boundary and footer-alignment edits compile, but were
not separately confirmed on the panel. Repair/unlock, error reset, LED control,
other pack models and other board revisions were not confirmed in this session.
A displayed health percentage is a software estimate, not a capacity/load test.

## License and credit

Original attribution and [LICENSE](LICENSE) are retained: PocketOBI uses
**PolyForm Noncommercial 1.0.0**; bundled components retain their own licenses,
including MIT for OneWire2. See [THIRD-PARTY.md](THIRD-PARTY.md). This port does
not grant commercial rights beyond the original terms.
