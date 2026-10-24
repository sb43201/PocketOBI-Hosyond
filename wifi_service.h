#pragma once
#include <Arduino.h>
#ifndef POCKETOBI_WIFI
#define POCKETOBI_WIFI 0
#endif
#if POCKETOBI_WIFI
void wifiSetup();
void wifiService();
void wifiRequestPortal();
bool wifiPortalActive();
bool wifiConnected();
String wifiNetwork();
String wifiAddress();
String wifiSetupName();
String wifiSetupPassword();
String wifiStatusLabel();
int wifiSignal();
#else
inline void wifiSetup() {}
inline void wifiService() {}
#endif
