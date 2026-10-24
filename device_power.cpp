#include "device_power.h"
#ifdef DEVICE_BATTERY_ADC_PIN
#include "pocketobi.h"

namespace {
float voltage = 0;
bool sampled = false;
unsigned long lastSample = 0;
void sample() {
  // Read the board's BAT_ADC divider, not the Makita communication lines.
  // ESP32's millivolt API applies the available ADC calibration.
  uint32_t millivolts = 0;
  analogReadMilliVolts(DEVICE_BATTERY_ADC_PIN); // discard first conversion
  for (int i = 0; i < 16; ++i)
    millivolts += analogReadMilliVolts(DEVICE_BATTERY_ADC_PIN);
  const float measured = millivolts / 16000.0f * DEVICE_BATTERY_DIVIDER * DEVICE_BATTERY_CALIBRATION;
  voltage = sampled ? voltage * 0.5f + measured * 0.5f : measured;
  sampled = true;
  lastSample = millis();
}
}
void devicePowerSetup() {
  pinMode(DEVICE_BATTERY_ADC_PIN, INPUT);
  analogReadResolution(12);
  analogSetPinAttenuation(DEVICE_BATTERY_ADC_PIN, ADC_11db);
  sample();
}
float deviceBatteryVoltage() { return voltage; }
bool deviceBatteryVoltageValid() { return sampled && voltage >= 0.5f && voltage <= 4.6f; }
bool devicePowerTick() {
  if (millis() - lastSample < 2000) return false;
  const bool wasValid = deviceBatteryVoltageValid();
  const int oldValue = (int)lroundf(voltage * 100);
  sample();
  return wasValid != deviceBatteryVoltageValid() || oldValue != (int)lroundf(voltage * 100);
}
void drawDevicePower() {
  char label[16];
  if (deviceBatteryVoltageValid()) snprintf(label, sizeof(label), "Unit %.2fV", voltage);
  else snprintf(label, sizeof(label), "Unit --");
  tft.fillRoundRect(DEVICE_BATTERY_BADGE_X, 4, DEVICE_BATTERY_BADGE_W, 24, 4, COL_PANEL);
  tft.setFont(NULL); tft.setTextSize(1);
  tft.setTextColor(deviceBatteryVoltageValid() && voltage < 3.3f ? COL_ORANGE : COL_TEXT, COL_PANEL);
  const int width = (int)strlen(label) * 6;
  tft.setCursor(DEVICE_BATTERY_BADGE_X + (DEVICE_BATTERY_BADGE_W - width) / 2, 12);
  tft.print(label);
}
#endif
