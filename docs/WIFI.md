# Wi-Fi feature branch

Branch: `codex/wifi-manager`. Based on the frozen `hosyond-v1.0.0` release.
The tagged release and main branch remain available without Wi-Fi.

## Build and upload

This branch defaults to **hosyond-cyd28-wifi**:

```powershell
pio run -e hosyond-cyd28-wifi
pio run -e hosyond-cyd28-wifi -t upload --upload-port COM8
```

COM8 is an example. Close Serial Monitor/browser serial connections first.
The original `hosyond-cyd28` environment still builds the offline firmware.
GPIO wiring remains **IO23 DATA / IO27 ENABLE**. Leave microSD and external
SPI accessories disconnected, as in the original release.

## First-time network setup

1. After normal touch calibration, the Wi-Fi build tries saved router credentials
   for up to about five seconds. With no successful connection it opens setup and
   shows the Wi-Fi information screen. Touchscreen navigation stays available.
2. Join the setup AP shown on the screen: the initial name is
   **PocketOBI-Setup-XXXXXX**. The six-character suffix depends on the device MAC.
3. Use the **AP key shown on the display** (initially `obi-XXXXXX`).
4. Open **http://192.168.4.1** if the captive portal does not open automatically.
5. In WiFiManager choose your **2.4 GHz** home network and enter its password.
6. The same form includes **Setup access point name** and **Setup access point
   password**. Change these if desired, then save. AP names accept 1-32 bytes;
   AP passwords accept 8-24 printable characters. These settings are separate
   from the router's SSID/password.
7. After connection, use the device IP shown on the touchscreen to open the
   PocketOBI browser dashboard from the same home network.

Router credentials are stored by WiFiManager. Setup AP settings are stored in a
separate NVS namespace. No router name/password is compiled into the firmware.
Setup AP name/password changes apply the next time the setup AP is opened; the
currently running portal keeps its current credentials until closed.

The portal closes after its configured three-minute inactivity window; a client
using it can extend that window. Without Wi-Fi, standalone battery functions
remain available. Network changes may take several seconds while connecting.

## Wi-Fi information tab

Open **Tools > Settings**, then tap the visible **Wi-Fi tab in the top-right
corner** of the title bar. Other parts of the title still act as Back.

The information screen shows:

- Connection/portal status.
- Router SSID, IP address, and signal strength.
- Setup AP name and AP key (router password is never displayed).
- Browser/setup address.

Tap **Open Wi-Fi setup** to change network credentials or the setup AP details.
Tap **Close Wi-Fi setup** to close the portal early. Tap the title bar to return
to Settings. The existing Settings rows keep their original sizes and touch
zones; there is no extra row hidden under calibration.

The Wi-Fi tab and information text are currently English, including when another
language is selected for the inherited PocketOBI screens.

## Browser functions

The dashboard is adapted from the earlier local OBI ESP32 project and uses this
port's proven PocketOBI functions, not the older port's OneWire implementation.

| Control | Behavior |
| --- | --- |
| Read Battery | Fresh battery information, voltage, temperature, status/verdict |
| Read Voltages | Fresh full read, keeping voltage and identity from the same acquisition |
| LEDs ON / OFF | Checks for a readable pack, then sends the LED command; visually verify the LEDs |
| Reset Errors | Explicit browser confirmation, fresh read, reset, then verification read |

A reset whose verification fails reports an unknown outcome, not success. A
still-locked pack is reported as still locked. Frame repair/unlock remains in the
existing touchscreen workflow; it has not been added as an unguarded web action.
No battery reads are triggered by background polling. Failed reads clear the
old displayed values instead of leaving stale measurements on screen.

The browser uses POST for battery operations and a same-origin custom header.
The application provides no remote-access login: use a trusted home LAN and do
not port-forward it. Setup AP access is protected by its displayed password.

## USB bridge and timing

WiFiManager debug output is disabled. The original normal firmware's USB bridge
remains available. Browser battery operations return a conflict response whenever
the PC bridge screen is selected, even if it is toggled OFF; return to the menu
before using browser battery controls.

Web requests are serviced synchronously in the main UI loop. Touchscreen,
web, and USB transactions therefore do not run concurrently on the battery bus.
The timing-sensitive OneWire code retains its original critical sections and
is not replaced by a second Wi-Fi-specific driver.

The setup portal and the battery dashboard both use port 80. The app server is
stopped before a setup portal opens and only restarts after WiFiManager releases
the port. The dashboard is not served while setup is active.

## Validation and next hardware checks

Firmware builds and browser/API contract tests are the automated checks. Run:

```powershell
node --test tests/web_ui.test.cjs
```

Hardware validation is still required for this feature branch:

1. First boot without saved router credentials; join the displayed AP and provision.
2. Confirm the home-network IP appears and the browser can read a known pack.
3. Perform repeated reads with Wi-Fi active and compare with touchscreen readings.
4. Change AP name/key, reopen setup and restart to verify persistence.
5. Try a wrong router password; confirm the touchscreen remains usable and setup
   can be reopened without erasing calibration or battery settings.
6. Confirm the portal-to-dashboard handoff and recovery after router disconnects.
7. Verify USB bridge reads still work; confirm browser battery requests are rejected
   while that screen is selected.
8. Rebuild/upload offline `hosyond-cyd28` to confirm the original workflow remains.

Repair/reset and LED hardware behavior remain subject to the original release's
validation limits. This branch does not enable ArduinoOTA or alter BMS thresholds.

## Recorded automated checks (October 23, 2026)

- `hosyond-cyd28-wifi`: build passed; RAM 47,440 bytes (14.5%), flash 980,053
  bytes (74.8% of the 1,310,720-byte application partition).
- `hosyond-cyd28`: offline regression build passed; RAM 22,552 bytes and flash
  355,341 bytes, matching the frozen release's size.
- `node --test tests/web_ui.test.cjs`: five tests passed, covering HTTP methods
  and headers, valid zero readings, bridge conflict handling, stale display
  clearing, LED form requests, reset confirmation, and pack changes on refresh.
- `git diff --check`: passed for the feature edits.

No firmware was uploaded and no live Wi-Fi provisioning or radio-active battery
reads were performed by these automated checks.
