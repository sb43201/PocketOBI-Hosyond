# PocketOBI Hosyond — User manual

Hosyond release 1.0.0 · October 23, 2026

## 1. Before use

This manual is for the Hosyond/LCDWiki **E32R28T ESP32-32E** with an ILI9341
screen and XPT2046 resistive touch. Use the normal **hosyond-cyd28** firmware.
No Wi-Fi connection, speaker, rotary encoder, or computer is needed for
standalone readings. USB supplies power. A computer is needed only for flashing
or the optional PC bridge.

Connect **IO23 to DATA, IO27 to ENABLE, and board GND to battery B-**. Both
signal pull-ups go to board 3V3. On the owner's tested adapter DATA is pin 6 and
ENABLE is pin 2; numbering is not universal. See the [README](README.md) wiring
section. Leave the microSD slot empty and disconnect external SPI accessories.
Never connect pack B+ or pack voltage to the board's power/battery connector.

## 2. Startup and calibration

1. Power the board through USB.
2. If crosshairs appear, tap the centre of each target and release before the
   next one. A stylus suitable for resistive touch or a fingernail can help.
3. After the saved message, use the main menu: Battery, Repair, Tools, About.
4. Calibration persists across resets and ordinary firmware uploads.

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

Changes save automatically. Tap the actual row or its value. The bottom
Title: back hint is aligned to the right; use the title bar to return to Tools.
Calibration temporarily uses the unflipped display orientation and then restores
your screen-flip setting.

## 7. USB PC bridge

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

Keep the same wiring in standalone and bridge modes. No Wi-Fi or network setup
is involved.

## 8. Repair and unlock

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

## 9. Troubleshooting

| Symptom | Check |
| --- | --- |
| White display | Upload current hosyond-cyd28 for E32R28T; this port fixes the ILI9341 CS/DC argument order |
| Battery tap opens About | Run touch calibration; do not rewire the battery to correct a touch problem |
| Footer opens calibration | Upload this release; Settings hit targets now exclude footer/gaps |
| Language changes unexpectedly; fictional results | Confirm the environment is not hosyond-cyd28-demo |
| No battery / all FF | Confirm pack is attached, common ground, correct DATA/ENABLE contacts, pull-ups, no SD/SPI accessory |
| USB connects but reads fail | Use normal release, ACTIVE bridge, close Serial Monitor, verify standalone read works |
| Upload cannot open port | Close browser serial connection and Serial Monitor; confirm current COM port |
| Guru Meditation screen | About-page Easter egg, not a real crash; tap to dismiss |

For a battery log, upload hosyond-cyd28-diagnostic, open Serial Monitor at
115200, reset, then tap Battery. Startup should report DATA=GPIO23 ENABLE=GPIO27
and demo=0. Copy the bus, cmd, ROM, and rx lines. Do not use the demo build for
real readings. A presence flag alone does not prove a valid battery response.

## 10. Updates and recovery

Keep the release source and tag together. Build/upload with:

```powershell
pio run -e hosyond-cyd28
pio run -e hosyond-cyd28 -t upload --upload-port COM8
```

COM8 is an example. PlatformIO handles the bootloader, partition and application
offsets. A firmware.bin produced by a build is an application image; do not
assume it is a merged image suitable for flashing at address 0x0.

Avoid erasing flash unless needed; saved calibration/settings otherwise remain.
If settings are erased, the normal firmware requests touch calibration again.

## 11. Provenance and testing

Based on TheRepairforge/PocketOBI e50539b (2.5.0), with the Hosyond adaptations
listed in [RELEASE_NOTES.md](RELEASE_NOTES.md). Original code and assets retain
their original attribution/licenses; see LICENSE and THIRD-PARTY.md.

The owner confirmed calibrated touch, repeated BL1850B reads, and USB bridge
battery reads. Settings hit-boundary/footer edits were compile-checked; repair,
reset and LED operations were not hardware-tested in this session. This release
has no Wi-Fi features.
