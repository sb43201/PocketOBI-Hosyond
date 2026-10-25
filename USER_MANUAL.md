# PocketOBI Hosyond — User manual

Hosyond Wi-Fi release 1.1.0 · October 25, 2026

## 1. Before use

This manual is for the Hosyond/LCDWiki **E32R28T ESP32-32E** with an ILI9341
screen and XPT2046 resistive touch. Use **hosyond-cyd28-wifi** (the default build).
Standalone touch readings do not require a Wi-Fi connection. Wi-Fi is used for
the optional browser dashboard. No speaker or rotary encoder is needed. Power
from USB or the board's dedicated single-cell battery connector. A computer is
needed for flashing or the USB PC bridge.

Connect **IO23 to DATA, IO27 to ENABLE, and board GND to battery B-**. Both
signal pull-ups go to board 3V3. On the owner's tested adapter DATA is pin 6 and
ENABLE is pin 2; numbering is not universal. See the [README](README.md) wiring
section. Leave the microSD slot empty and disconnect external SPI accessories.
Never connect pack B+ or pack voltage to the board's power/battery connector.

## 2. Startup and calibration

1. Power through USB or a correctly connected single-cell battery.
2. If crosshairs appear, tap the centre of each target and release before the
   next one. A stylus suitable for resistive touch or a fingernail can help.
3. After the saved message, use the main menu: Battery, Repair, Tools, About.
4. Calibration persists across resets and ordinary firmware uploads.
5. If no router connects, the Wi-Fi information screen shows the setup AP name
   and key. Join it and open http://192.168.4.1 to enter router credentials.
   See the Wi-Fi section below. You can return to the menus while offline.

If tap locations are wrong, open Tools > Settings > Calibrate touch. If the
menus cannot be reached, restart normally, then press and hold BOOT during the
splash; release when the calibration target appears. Do not hold BOOT while
resetting or powering on, because that enters the ESP32 download mode. This
startup calibration shortcut is skipped when PC bridge at startup is enabled.

## 3. Touch controls

| Screen or area | Action |
| --- | --- |
| Main menu tile | Open the labeled function |
| Title bar on a subpage | Back to the previous menu; footer says Title: back |
| Battery page left/right edges | Previous/next battery page |
| Battery page centre | Read the battery again |
| Tools visible row | Open that tool |
| Tools band below the rows, above the footer | Switch between the two tool pages |
| Settings Wi-Fi tab, beside the Unit badge | Open Wi-Fi information/setup |
| Settings visible row | Toggle/cycle that setting, or open calibration |
| Settings margins, row gaps, footer | No setting action |
| PC bridge body | Toggle ACTIVE/OFF |
| About body | Cycle retro-computer Easter egg messages |

Use deliberate taps and release between taps. The bottom hints describe the
controls; they are not a row of independent buttons. In particular, Title: back
means tap the **top title bar**, not the hint itself.

## 4. Read a battery

1. Attach the wired adapter securely to a supported Makita LXT pack.
2. Tap Battery. A read takes several seconds; wait for the result.
3. Use the side edges to browse Overview, Health, and Identity.
4. Tap the centre to refresh; tap the title bar to return to the menu.

| View | Information |
| --- | --- |
| Overview | Cell voltages, total pack voltage, cell spread |
| Health | Temperature readings, wear/cycle-based estimates, fault assessment |
| Identity | Reported model, charge count, manufacturing data, capacity, ROM ID |

The exact fields depend on the pack/BMS generation. A charge counter is the
reported BMS counter. Capacity is decoded metadata, not a measured discharge
capacity. The health percentage and verdict are software assessments from the
available data; they do not replace a load or capacity test.

A healthy-looking pack does not need an error reset or unlock. An error/status
byte alone should not be interpreted as a fault; use the decoded lock fields and
verdict. For example, the validated pack reported err=0x45 while its lock fields
were zero and its firmware verdict was HEALTHY.

## 5. Tools

Tools has two pages. The first contains PC bridge, raw debug, and Settings;
the second contains pack LEDs on, pack LEDs off, and error reset. Tap below the
three visible rows to change page.

**Pack LEDs:** sends the upstream LED command to compatible packs. It controls
the battery's indicators, not the board's backlight. Hardware behavior was not
verified in this port's testing session.

**Raw debug:** displays the battery ROM/frame for troubleshooting.

**Error reset:** a separate BMS operation from unlocking. The firmware asks for
confirmation and attempts to reread afterward. A failed reread means the result
is unknown, not that the reset succeeded. This operation was not verified here.

## 6. Settings

| Setting | Behavior |
| --- | --- |
| Flip screen | Rotate display and touch mapping by 180 degrees |
| PC bridge at boot | Open the bridge screen automatically after restart |
| Language | Cycle available languages; normal build offers EN/FR/DE/ES |
| Calibrate touch | Three-point touch calibration |
| Wi-Fi tab in title bar | Router/AP information and setup portal controls |

Changes save automatically. Tap the actual row or its value. The bottom
Title: back hint is aligned to the right; use the title bar to return to Tools.
Calibration temporarily uses the unflipped display orientation and then restores
your screen-flip setting.

## 7. Wi-Fi setup and browser dashboard

1. Boot the device. If a saved router connects, its IP appears in Wi-Fi information.
2. Otherwise join the setup AP shown on the screen, initially
   **PocketOBI-Setup-XXXXXX**, using its displayed AP key (`obi-XXXXXX` initially).
3. Open **http://192.168.4.1**, choose your 2.4 GHz network and enter its password.
4. You can also customize the **setup AP name** (1-32 bytes) and **setup AP key**
   (8-24 printable characters). These are separate from the router credentials
   and take effect the next time you open setup.
5. After saving and connecting, put your phone/PC on that router and open the IP
   shown in **Tools > Settings > Wi-Fi**.

The Wi-Fi tab sits immediately left of the Unit-voltage badge. Its page shows
status, router SSID, IP, RSSI, setup AP name/key and the browser address. Tap
Open Wi-Fi setup to change details, or Close Wi-Fi setup to end a portal session.
The portal closes after about three minutes of inactivity; an active client can
extend that interval. Connection attempts can pause processing for a few seconds.

The browser has Read Battery, Read Voltages, LEDs ON/OFF and Reset Errors.
Read Voltages uses a fresh full read so the identity matches the current pack.
Reset requires confirmation and reports a verification failure as unknown, rather
than claiming success. LED success means the command was sent; check the actual
pack LEDs. Repair/unlock remains on the touchscreen.

No battery operations run in background polling. Requests execute one at a time
on the same working battery driver as the touchscreen. While the PC bridge screen
is selected, browser battery requests are rejected; exit it before web use.

Network credentials stay in device storage. The router password is not shown on
the screen or status API. The dashboard has no remote-access login; use a trusted
LAN and do not port-forward it. See [Wi-Fi guide](docs/WIFI.md) for details.

## 8. Device Li-ion voltage

The top-right **Unit x.xxV** badge shows the handheld's own BAT+ voltage, not the
Makita pack voltage. It uses GPIO34 and the onboard divider, samples between bus
transactions and updates about every two seconds. Below 3.3 V the text is orange;
there is no automatic power cutoff. Long titles are shortened to keep the badge
and page dots clear.

Use the board's dedicated **single-cell 3.7 V Li-ion/Li-poly connector**, checking
polarity. Never connect the Makita pack to that connector. If powering USB from
an external boost converter, the badge does not measure that external cell.

The badge is a voltage monitor, not a battery percentage or charging/full signal.
USB charging may energize BAT+ even with no cell connected. The owner confirmed
the display works; absolute accuracy still needs a multimeter comparison.
See [device power guide](docs/DEVICE_POWER.md) for the circuit and calibration.

## 9. USB PC bridge

This mode lets compatible OBI software use the Hosyond as a USB-to-battery
adapter. The normal firmware was confirmed to read batteries through the OBI
browser tool.

1. Attach the pack and connect the board to the PC with a data-capable USB cable.
2. Close PlatformIO Serial Monitor and any other program holding the COM port.
3. On the board, open Tools > PC bridge and confirm ACTIVE.
4. Open [OBI-1](https://openbatteryinformation.github.io/) in a supported desktop
   browser and choose the board's serial port when connecting.
5. If the browser resets the board, re-enter PC bridge, or enable PC bridge at
   boot in Settings before reconnecting.
6. Read the battery through the PC application. Do not tap the bridge body during
   the request: that toggles the bridge off.
7. Disconnect the browser before opening Serial Monitor or uploading firmware.

For diagnostics the board's serial rate is 115200. The normal release emits no
battery transaction text. The diagnostic build suppresses that text in bridge
mode so it cannot mix with binary replies; its standalone startup logging may
still appear when the board reboots. Prefer the normal release for PC use.

Keep the same wiring in standalone and bridge modes. The USB bridge does not require a Wi-Fi connection. Exit bridge mode before
using browser battery controls.

## 10. Repair and unlock

Repair reads the pack and applies the upstream staged fault/lock assessment.
When allowed, a separate confirmation screen precedes the write operation.
The firmware includes checks intended to block unlocking when it detects a
hardware fault. These checks are not a guarantee that a pack is safe.

Do not unlock a pack with damaged cells, a broken sense connection, abnormal
heating, or another unresolved physical fault. Do not use an unlock simply to
suppress a genuine fault. Repair/unlock and its claimed safeguards have not been
hardware-validated in this Hosyond port. Consult upstream's documentation and
a qualified battery repairer if you are unsure. Use Cancel or the title bar to
leave the confirmation screen without writing.

## 11. Troubleshooting

| Symptom | Check |
| --- | --- |
| White display | Upload current hosyond-cyd28-wifi for E32R28T; this port fixes the ILI9341 CS/DC argument order |
| Battery tap opens About | Run touch calibration; do not rewire the battery to correct a touch problem |
| Footer opens calibration | Upload this release; Settings hit targets now exclude footer/gaps |
| Language changes unexpectedly; fictional results | Confirm the environment is not hosyond-cyd28-demo |
| No battery / all FF | Confirm pack is attached, common ground, correct DATA/ENABLE contacts, pull-ups, no SD/SPI accessory |
| USB connects but reads fail | Use normal release, ACTIVE bridge, close Serial Monitor, verify standalone read works |
| Upload cannot open port | Close browser serial connection and Serial Monitor; confirm current COM port |
| Wi-Fi setup page will not open | Join the displayed setup AP with its AP key, then use http://192.168.4.1 |
| Browser cannot read while USB bridge is selected | Exit the PC bridge screen; do not run both controllers at once |
| Unit voltage seems incorrect | Check connection to the board BAT connector and compare with a multimeter; see device power guide |
| Guru Meditation screen | About-page Easter egg, not a real crash; tap to dismiss |

For a battery log, upload hosyond-cyd28-diagnostic, open Serial Monitor at
115200, reset, then tap Battery. Startup should report DATA=GPIO23 ENABLE=GPIO27
and demo=0. Copy the bus, cmd, ROM, and rx lines. Do not use the demo build for
real readings. A presence flag alone does not prove a valid battery response.

## 12. Updates and recovery

Keep the release source and tag together. Build/upload with:

```powershell
pio run -e hosyond-cyd28-wifi
pio run -e hosyond-cyd28-wifi -t upload --upload-port COM8
```

COM8 is an example. PlatformIO handles the bootloader, partition and application
offsets. A firmware.bin produced by a build is an application image; do not
assume it is a merged image suitable for flashing at address 0x0.

Avoid erasing flash unless needed; saved calibration/settings otherwise remain.
If settings are erased, the normal firmware requests touch calibration again.

## 13. Provenance and testing

Based on TheRepairforge/PocketOBI e50539b (2.5.0), with the Hosyond adaptations
listed in [RELEASE_NOTES.md](RELEASE_NOTES.md). Original code and assets retain
their original attribution/licenses; see LICENSE and THIRD-PARTY.md.

The owner confirmed calibrated touch, repeated BL1850B reads, USB bridge battery
reads, the Wi-Fi functions and the unit-voltage display. This does not establish
coverage of every pack model, repair/reset failure scenario, ADC accuracy or
charging condition. Five browser/API regression tests pass; both Wi-Fi and
offline environments build. The original upstream project is
[TheRepairforge/PocketOBI](https://github.com/TheRepairforge/PocketOBI). Retained
licenses and attribution apply; this is an independent, noncommercial port.
