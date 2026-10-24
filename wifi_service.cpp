#include "wifi_service.h"
#if POCKETOBI_WIFI
#include "pocketobi.h"
#include "decode.h"
#include "protocol.h"
#include "display.h"
#include <WiFi.h>
#include <WiFiManager.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "web_interface.h"
#include "device_power.h"

namespace {
WiFiManager manager;
WebServer web(80);
WiFiManagerParameter apNameField("apname", "Setup access point name (1-32 bytes)", "", 32, "required minlength=\"1\" maxlength=\"32\"");
WiFiManagerParameter apPassField("appass", "Setup access point password (8-24 characters)", "", 24, "required minlength=\"8\" maxlength=\"24\"");
String setupName, setupPassword, activeName, activePassword;
bool webRunning = false, portalRequested = false;
unsigned long lastInfoDraw = 0;

bool printable(const String &s) {
  for (size_t i = 0; i < s.length(); ++i)
    if ((uint8_t)s[i] < 32 || (uint8_t)s[i] == 127) return false;
  return true;
}
void savePortalSettings() {
  String name(apNameField.getValue()), password(apPassField.getValue());
  name.trim();
  if (!name.length() || name.length() > 32 || !printable(name) ||
      password.length() < 8 || password.length() > 24 || !printable(password)) return;
  // Only AP settings live here. Router credentials are managed by WiFiManager.
  Preferences storage;
  storage.begin("pocketobi-wifi", false);
  storage.putString("apname", name);
  storage.putString("appass", password);
  storage.end();
  setupName = name; setupPassword = password;
}
void jsonReply(JsonDocument &doc, int code = 200) {
  String body;
  serializeJson(doc, body);
  web.sendHeader("Cache-Control", "no-store");
  web.send(code, "application/json", body);
}
void errorReply(int code, const char *message) {
  JsonDocument doc;
  doc["success"] = false; doc["error"] = message;
  jsonReply(doc, code);
}
bool batteryAllowed() {
  // A custom same-origin header prevents cross-site form/navigation commands.
  // No CORS permission is given. All handlers run on the UI loop, never async.
  if (web.header("X-PocketOBI") != "1") {
    errorReply(403, "Use the PocketOBI page to issue battery commands."); return false;
  }
  if (state == PC_BRIDGE) {
    errorReply(409, "USB PC bridge is selected. Exit bridge mode on the touchscreen first."); return false;
  }
  if (state == CONFIRM_RESET || state == CONFIRM_UNLOCK) {
    errorReply(409, "Finish or cancel the touchscreen confirmation first."); return false;
  }
  return true;
}
void batteryReply(bool success) {
  JsonDocument doc;
  doc["success"] = success;
  if (!success) {
    doc["error"] = "No valid battery response. Check that the pack is connected.";
    jsonReply(doc); return;
  }
  doc["model"] = bat.model;
  doc["locked"] = bat.locked;
  doc["chargerLocked"] = bat.chargerLocked;
  doc["chargeCount"] = bat.chargeCount;
  char date[16];
  snprintf(date, sizeof(date), "%04d-%02d-%02d", bat.mfgYear, bat.mfgMonth, bat.mfgDay);
  doc["mfgDate"] = date;
  doc["capacity"] = bat.capacityAh;
  doc["errorCode"] = bat.errorCode;
  doc["packVoltage"] = bat.packVoltage;
  for (int i = 0; i < cellCount; ++i) doc[String("cell") + (i + 1)] = bat.cell[i];
  doc["cellCount"] = cellCount;
  doc["cellDiff"] = bat.cellDiff;
  doc["tempCell"] = bat.tempCell;
  if (bat.boardTempValid) doc["tempMosfet"] = bat.tempMosfet;
  else doc["tempMosfet"] = nullptr;
  doc["verdict"] = verdictText(computeVerdict());
  doc["healthEstimate"] = bat.healthEstPct;
  char serial[17]; formatSerial(serial); doc["serial"] = serial;
  jsonReply(doc);
}
void refreshScreen() {
  lastRenderedState = -1;
  render();
}
void readBattery() {
  if (!batteryAllowed()) return;
  const bool ok = readAllData();
  if (ok) readExtended();
  batteryReply(ok);
  refreshScreen();
}
void leds() {
  if (!batteryAllowed()) return;
  if (!web.hasArg("state") || (web.arg("state") != "0" && web.arg("state") != "1")) {
    errorReply(400, "LED state must be 0 or 1."); return;
  }
  if (!readAllData()) { errorReply(422, "No valid battery response; LED command not sent."); return; }
  ledsSet(web.arg("state") == "1");
  JsonDocument doc;
  doc["success"] = true;
  doc["message"] = "LED command sent. Check the pack LEDs to confirm.";
  jsonReply(doc);
  refreshScreen();
}
void resetBattery() {
  if (!batteryAllowed()) return;
  if (web.arg("confirm") != "reset") { errorReply(400, "Reset confirmation is required."); return; }
  if (!readAllData()) { errorReply(422, "No valid battery response; reset not sent."); return; }
  resetLockedBefore = bat.locked;
  resetErrors();
  resetReread = readAllData();
  resetLockedAfter = bat.locked;
  if (resetReread) readExtended();
  JsonDocument doc;
  doc["success"] = resetReread && !resetLockedAfter;
  doc["reread"] = resetReread;
  doc["lockedBefore"] = resetLockedBefore;
  if (resetReread) doc["lockedAfter"] = resetLockedAfter;
  else doc["lockedAfter"] = nullptr;
  doc["message"] = !resetReread ? "Reset sent, but verification read failed: outcome unknown."
    : resetLockedAfter ? "Reset sent; the battery still reports locked."
    : "Verification read succeeded; the BMS fault lock is clear.";
  jsonReply(doc);
  refreshScreen();
}
void status() {
  JsonDocument doc;
  doc["success"] = true;
  doc["connected"] = wifiConnected();
  doc["ssid"] = wifiNetwork();
  doc["ip"] = wifiAddress();
  doc["rssi"] = wifiSignal();
  doc["setupAp"] = setupName;
  if (deviceBatteryVoltageValid()) doc["deviceBatteryVoltage"] = deviceBatteryVoltage();
  else doc["deviceBatteryVoltage"] = nullptr;
  doc["bridge"] = state == PC_BRIDGE;
  jsonReply(doc);
}
void routes() {
  const char *headers[] = { "X-PocketOBI" };
  web.collectHeaders(headers, 1);
  web.on("/", HTTP_GET, [] { web.send_P(200, "text/html", INDEX_HTML); });
  web.on("/api/status", HTTP_GET, status);
  web.on("/api/read", HTTP_POST, readBattery);
  // A full read keeps model, verdict and voltages from the same acquisition.
  web.on("/api/voltages", HTTP_POST, readBattery);
  web.on("/api/leds", HTTP_POST, leds);
  web.on("/api/reset", HTTP_POST, resetBattery);
  web.onNotFound([] { errorReply(404, "Not found. Battery actions require POST."); });
}
}

bool wifiPortalActive() { return manager.getConfigPortalActive(); }
bool wifiConnected() { return WiFi.status() == WL_CONNECTED; }
String wifiNetwork() { return wifiConnected() ? WiFi.SSID() : String("Not connected"); }
String wifiAddress() { return wifiPortalActive() ? WiFi.softAPIP().toString() : wifiConnected() ? WiFi.localIP().toString() : String("--"); }
String wifiSetupName() { return wifiPortalActive() ? activeName : setupName; }
String wifiSetupPassword() { return wifiPortalActive() ? activePassword : setupPassword; }
String wifiStatusLabel() { return wifiPortalActive() ? "Setup portal active" : wifiConnected() ? "Connected" : "Not connected"; }
int wifiSignal() { return wifiConnected() ? WiFi.RSSI() : 0; }
void wifiRequestPortal() { portalRequested = true; }

void wifiSetup() {
  char suffix[7];
  snprintf(suffix, sizeof(suffix), "%06X", (unsigned)((ESP.getEfuseMac() >> 24) & 0xFFFFFF));
  Preferences storage;
  storage.begin("pocketobi-wifi", true);
  setupName = storage.getString("apname", String("PocketOBI-Setup-") + suffix);
  setupPassword = storage.getString("appass", String("obi-") + suffix);
  storage.end();
  apNameField.setValue(setupName.c_str(), 32);
  apPassField.setValue(setupPassword.c_str(), 24);
  manager.setDebugOutput(false); // Never contaminate the USB bridge.
  manager.setHostname("pocketobi");
  manager.setConfigPortalBlocking(false);
  manager.setConnectTimeout(5);
  manager.setConfigPortalTimeout(180);
  manager.setSaveParamsCallback(savePortalSettings);
  manager.addParameter(&apNameField);
  manager.addParameter(&apPassField);
  routes();
  WiFi.setAutoReconnect(true);
  activeName = setupName; activePassword = setupPassword;
  manager.autoConnect(activeName.c_str(), activePassword.c_str());
  // Do not start our port-80 server until WiFiManager releases its own.
  if (wifiPortalActive() && state != PC_BRIDGE) state = WIFI_INFO;
}
void wifiService() {
  if (portalRequested && state != PC_BRIDGE) {
    portalRequested = false;
    if (webRunning) { web.stop(); webRunning = false; }
    if (wifiPortalActive()) {
      manager.stopConfigPortal();
      lastRenderedState = -1;
    } else {
      apNameField.setValue(setupName.c_str(), 32);
      apPassField.setValue(setupPassword.c_str(), 24);
      activeName = setupName; activePassword = setupPassword;
      manager.startConfigPortal(activeName.c_str(), activePassword.c_str());
      lastRenderedState = -1;
    }
  }
  if (wifiPortalActive()) {
    manager.process();
  } else if (wifiConnected()) {
    if (!webRunning) { web.begin(); webRunning = true; }
    web.handleClient();
  } else if (webRunning) {
    web.stop(); webRunning = false;
  }
  if (state == WIFI_INFO && millis() - lastInfoDraw >= 1500) {
    lastInfoDraw = millis(); lastRenderedState = -1; render();
  }
}
#endif
