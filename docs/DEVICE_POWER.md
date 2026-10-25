# Device Li-ion voltage display

Included in Hosyond Wi-Fi release 1.1.0. The owner confirmed the Wi-Fi functions
and device-voltage display work.
Build/upload **hosyond-cyd28-wifi** as before. The offline hosyond-cyd28 build on
this branch also includes the unit-voltage indicator; the 1.0.0 tag is unchanged.

## Power connection

Use the E32R28T board's dedicated **single-cell 3.7 V Li-ion/Li-poly battery
connector**, matching the connector's actual + and - markings. The board has an
onboard single-cell charging circuit. Do not assume another battery cable's
wire order matches this connector. Do not connect the Makita pack to it.

This feature senses the board's BAT+ rail. If using an external boost converter
or power bank to supply USB instead, it does not measure that external cell.

## What is displayed

**Unit 3.85V** in the top-right header is the handheld's own BAT+ voltage. The
Makita pack's total/cell readings are unchanged. Page dots are shifted left and
the Settings Wi-Fi tab sits immediately left of the unit badge. Long screen
titles are shortened with an ellipsis so they do not cover these controls.

The value is averaged over 16 calibrated-millivolt ADC conversions and refreshed
about every two seconds while the main loop is available, with modest smoothing.
No sampling is inserted into OneWire transactions. Below 3.3 V the text is orange;
this is an indicator only, not an automatic shutdown or protection circuit.
`Unit --` means no plausible measurement is available. Boot splash, calibration
screens, and the About fake-crash screen are not normal headers.

Voltage is **not** a state-of-charge percentage or charging/full indication.
When USB is connected, the charger can energize BAT+ even with no cell attached;
a displayed voltage does not prove a cell is connected. Values while charging
may differ from the rested cell voltage.

## Circuit and conversion

The manufacturer's [E32R28T schematic](https://www.lcdwiki.com/res/E32R28T/2.8inch_ESP32-32_Display_Schematic.pdf)
shows BAT+ through **R2 = 100 kilohms** to BAT_ADC, with **R3 = 100 kilohms** from
BAT_ADC to ground. The [pin table](https://www.lcdwiki.com/2.8inch_ESP32-32E_Display#ESP32_Pin_Assignment)
assigns BAT_ADC to **GPIO34 (ADC1)**. No external sense wire/divider is needed for
this board's battery connector. ADC1 remains available with Wi-Fi enabled.

Conversion: `BAT volts = ADC millivolts / 1000 * 2 * calibration factor`.
Firmware uses 12-bit ADC readings and 11 dB attenuation. GPIO23 DATA and GPIO27
ENABLE are unchanged.

## Verify accuracy

After uploading, compare the displayed value with a multimeter across the
connected cell/board BAT connector, preferably with USB disconnected. ADC and
resistor tolerances mean this is an approximate monitor, not a precision meter.
Absolute voltage accuracy has not yet been independently compared with a meter.

If a repeatable correction is needed, add a build flag such as
`-D DEVICE_BATTERY_CALIBRATION=1.02f` to the Wi-Fi environment's build_flags.
The correction factor is `multimeter voltage / uncorrected displayed voltage`.
Rebuild and check again. Do not change the factor just to make a charging cell
look like an expected nominal voltage.

The status API also includes `deviceBatteryVoltage` (or null for an implausible
reading); it does not expose a fabricated battery percentage or charge state.

## Build checks

Both `hosyond-cyd28-wifi` and `hosyond-cyd28` compiled after this change. Wi-Fi
build: 47,640 bytes RAM / 989,253 bytes flash. Offline build: 22,736 bytes RAM /
364,141 bytes flash. The five browser regression tests passed. The owner subsequently confirmed the display works. Absolute voltage accuracy
and charging behavior have not been independently verified; these software
checks do not replace that measurement.
