#pragma once
#include <Arduino.h>
#include "board_config.h"

// Layout keeps page indicators and the Settings Wi-Fi tab left of the badge.
constexpr int DEVICE_BATTERY_BADGE_X = 240;
constexpr int DEVICE_BATTERY_BADGE_W = 72;
constexpr int SETTINGS_WIFI_TAB_X = 172;
constexpr int SETTINGS_WIFI_TAB_W = 60;

#ifdef DEVICE_BATTERY_ADC_PIN
void devicePowerSetup();
bool devicePowerTick();
void drawDevicePower();
float deviceBatteryVoltage();
bool deviceBatteryVoltageValid();
#else
inline void devicePowerSetup() {}
inline bool devicePowerTick() { return false; }
inline void drawDevicePower() {}
inline float deviceBatteryVoltage() { return 0.0f; }
inline bool deviceBatteryVoltageValid() { return false; }
#endif
