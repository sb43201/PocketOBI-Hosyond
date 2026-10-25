# PocketOBI for Hosyond 2.8-inch ESP32

**Hosyond Wi-Fi release 1.1.0 — October 25, 2026**

Standalone Makita LXT battery reader for the **Hosyond/LCDWiki E32R28T
ESP32-32E** display board. Includes calibrated touch, a WiFiManager setup portal,
a browser dashboard, a USB PC bridge, and the handheld's own Li-ion voltage display.
Based on [TheRepairforge/PocketOBI](https://github.com/TheRepairforge/PocketOBI),
upstream commit `e50539b` (2.5.0). This is a hardware port, not an official
TheRepairForge release.

[User manual](USER_MANUAL.md) · [Wi-Fi setup](docs/WIFI.md) ·
[Device battery power](docs/DEVICE_POWER.md) · [Release notes](RELEASE_NOTES.md) ·
[Original upstream README](docs/UPSTREAM_README.md) · [License](LICENSE)

## Working configuration

| Item | Configuration |
| --- | --- |
| Board | LCDWiki E32R28T / ESP32-WROOM-32E, 4 MB flash |
| Display | ILI9341V, 320 x 240 landscape, backlight GPIO21 |
| Touch | XPT2046 resistive touchscreen, saved three-point calibration |
| Battery DATA | **GPIO23**, exposed SPI MOSI pin |
| Battery ENABLE | **GPIO27**, exposed SPI CS pin |
| Firmware environment | **hosyond-cyd28-wifi** (default) |
| Wi-Fi | 2.4 GHz; WiFiManager provisioning; local browser dashboard |
| Unit battery sensing | GPIO34 ADC1, onboard 100k/100k divider |
| Speaker | Not used |

This is **not** the ESP32-S3 ES3C28P/ES3N28P board. Generic older CYD CN1/P3
wiring does not describe this board. Use the
[LCDWiki pin assignment](https://www.lcdwiki.com/2.8inch_ESP32-32E_Display#ESP32_Pin_Assignment).

## Wiring

Disconnect USB and the pack before changing wiring. Power the board through USB
or its dedicated single-cell 3.7 V Li-ion/Li-poly connector, checking polarity.
**Never connect Makita B+ to the board or to its single-cell battery connector.**

| Board | Battery adapter |
| --- | --- |
| IO23 / SPI MOSI | DATA — pin 6 on the owner's tested adapter |
| IO27 / SPI CS | ENABLE — pin 2 on the owner's tested adapter |
| GND | Main battery B- |
| 3V3 | Supply for DATA and ENABLE pull-up resistors |

Fit one pull-up resistor from DATA to 3V3 and another from ENABLE to 3V3.
Upstream specifies **470 ohms**; 4.7 kilohms may work with short wiring but can
be marginal. Actual resistor values on the tested unit were not independently measured. Identify contacts by function: adapter numbering can differ.

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

The top bar also displays **Unit x.xxV**, the device's own single-cell battery
voltage measured through the board's built-in GPIO34 divider. It is separate
from the Makita pack voltage. See [device battery power](docs/DEVICE_POWER.md).

## Build and upload

Install PlatformIO, open this repository folder, and select **`hosyond-cyd28-wifi`**.

```powershell
pio run -e hosyond-cyd28-wifi
pio run -e hosyond-cyd28-wifi -t upload --upload-port COM8
```

Replace COM8 with the board's current port. Close Serial Monitor and browser
serial connections before uploading. PlatformIO flashes all required images at
the correct offsets; do not flash an application-only `firmware.bin` at 0x0.

On first startup after the calibration update, tap each of the three crosshairs
and release between taps. Settings are retained across ordinary uploads.

## First Wi-Fi setup

1. Boot the device. If no saved network connects, it shows its setup AP name/key.
2. Join **PocketOBI-Setup-XXXXXX**, using the **AP key on the touchscreen**.
3. Open **http://192.168.4.1** if the captive portal does not open automatically.
4. Select your 2.4 GHz router and enter its password. You can also change the
   setup AP name/key; those changes apply next time setup opens.
5. Once connected, open the device IP shown in **Tools > Settings > Wi-Fi**.

The browser dashboard reads battery data/voltages, sends pack LED commands and
provides confirmed error reset with a verification read. Frame repair/unlock
stays in the touchscreen workflow. Router credentials are entered on the device's
portal and saved locally; no personal Wi-Fi credentials are included in this repo.

The dashboard is for a trusted local LAN; it has no remote-access login. Do not
port-forward it. The setup AP has a device-specific initial key displayed on the
screen; change it in the portal if desired. See [Wi-Fi guide](docs/WIFI.md).

## Use

- Tap **Battery** to read the pack. Tap left/right edges to change pages; tap the
  centre to refresh. Tap the title bar to go back.
- **Tools > PC bridge** opens the USB bridge. Keep it ACTIVE, close Serial Monitor,
  then connect from the OBI browser tool. This workflow was confirmed by the owner.
- In Tools, tap the band below the three rows to change tool pages.
- **Tools > Settings** includes screen flip, bridge at startup, language, and touch
  calibration. The **Wi-Fi tab**, just left of the unit-voltage badge, opens network
  information and setup controls. Only visible Settings rows act; footer/gap taps do nothing.

Read the [manual](USER_MANUAL.md) before using repair/reset functions.

## Build variants

| Environment | Purpose |
| --- | --- |
| **hosyond-cyd28-wifi** | Recommended/default: Wi-Fi, touch, USB bridge and unit voltage |
| hosyond-cyd28 | Offline build with touch, USB bridge and unit voltage |
| hosyond-cyd28-diagnostic | Same hardware with battery logs at 115200; transaction logs are muted in PC bridge |
| hosyond-cyd28-demo | Fictional battery screen tour; does not read or repair a real battery |

Other upstream environments remain in platformio.ini but are not covered by this
Hosyond release's hardware validation.

## Validation and limits

The owner reported working display/touch, Wi-Fi setup and browser functions,
USB PC bridge battery reads, and the unit-voltage indicator on the E32R28T.
Captured BL1850B reads were repeatable, around 20 V with 1-2 mV cell spread.
This is not a comprehensive test of every pack model or fault/repair scenario.
The unit-voltage display works, but absolute accuracy has not been checked against
a calibrated meter. A health percentage is an estimate, not a capacity/load test.

Both Wi-Fi and offline firmware compile; five browser/API regression tests pass.
Run them with `node --test tests/web_ui.test.cjs`. Hardware validation reports are
specific to this board/setup. Source version labels from upstream are retained;
the **hosyond-v1.1.0** Git tag identifies this port release. The earlier offline
baseline remains available as [hosyond-v1.0.0](https://github.com/sb43201/PocketOBI-Hosyond/releases/tag/hosyond-v1.0.0).

## Original projects and attribution

- **TheRepairForge — [PocketOBI](https://github.com/TheRepairforge/PocketOBI)**:
  the original standalone battery firmware, protocol/decode, UI, repair flow and
  assets. This port is based on [commit e50539b](https://github.com/TheRepairforge/PocketOBI/commit/e50539b)
  (upstream 2.5.0), not a claim to authorship of the original project.
- **Martin Jansson — [Open Battery Information](https://github.com/mnh-jansson/open-battery-information)**:
  battery communication research and the base OBI interface/protocol.
- **[appositeit/obi-esp32](https://github.com/appositeit/obi-esp32)**: source of the
  browser interface adapted through the earlier OBI ESP32 project. Its MIT notice
  is retained in [docs/licenses/obi-esp32-MIT.txt](docs/licenses/obi-esp32-MIT.txt).
- **[tzapu/WiFiManager](https://github.com/tzapu/WiFiManager)** and
  **[ArduinoJson](https://arduinojson.org/)**: network provisioning and JSON dependencies.

Hosyond-specific changes are hardware/display/GPIO adaptation, touch fixes,
serial-safe diagnostics, Wi-Fi integration and the device battery monitor.
This independent port is not endorsed by the upstream authors or Makita.

## License

Original attribution and [LICENSE](LICENSE) are retained: PocketOBI uses
**PolyForm Noncommercial 1.0.0**; bundled components retain their own licenses,
including MIT for OneWire2. See [THIRD-PARTY.md](THIRD-PARTY.md). This port does
not grant commercial rights beyond the original terms.
