// ESP32 Relay Control System - Main File
// This file contains only setup() and loop() functions
// Other functionality is split into separate .ino files
// 
// IMPORTANT: Arduino IDE processes this file first, then others alphabetically
// All includes must be here for proper compilation

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <time.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <Update.h>
// Optional display support - comment out if not using display
// #define USE_DISPLAY  // For Adafruit libraries (SSD1306/SH1106)
#define USE_DISPLAY_U8G2  // For U8g2 library (more compatible)

#ifdef USE_DISPLAY
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
#endif

#ifdef USE_DISPLAY_U8G2
  #include <U8g2lib.h>
  #include <Wire.h>
#endif

// Pin definitions
#define RELE1_PIN 26
#define RELE2_PIN 27
#define LDR_PIN 34

// Global variables declarations
extern WebServer server;
extern Preferences preferences;
extern bool rele1State;
#ifdef USE_RELAY2
extern bool rele2State;
#endif
extern int ldrValue;
extern int ldrThreshold;
extern bool ntpSynced;
extern unsigned long currentMillis;
extern unsigned long lastLDRRead;
extern unsigned long lastTimeCheck;
extern bool wifiConnected;
extern bool needsWiFiReconnect;
extern bool needsSaveConfig;
extern unsigned long lastReconnectAttempt;
extern unsigned long lastConfigSave;
extern unsigned long lastStatusPrint;
extern int wifiReconnectAttempts;
extern const char* ntpServer;
extern const long gmtOffset_sec;
extern const int daylightOffset_sec;
extern char ap_ssid[32];
extern const char* ap_password;

// Operation modes enum
enum OperationMode {
  MODE_MANUAL = 0,
  MODE_AUTO_LDR = 1,
  MODE_SCHEDULED = 2
};
extern OperationMode currentMode;

// Structures
struct TimeSchedule {
  int onHour = 8;
  int onMinute = 0;
  int onSecond = 0;
  int offHour = 20;
  int offMinute = 0;
  int offSecond = 0;
};
extern TimeSchedule schedule;

struct WiFiConfig {
  char ssid[32];
  char password[64];
  bool useStaticIP;
  IPAddress staticIP;
  IPAddress gateway;
  IPAddress subnet;
  IPAddress dns1;
  IPAddress dns2;
};
extern WiFiConfig currentConfig;

// Function declarations
void initializeHardware();
void initializeNetwork();
void initializeDisplay();
void updateDisplay();
void displayMessage(const char* line1, const char* line2, 
                   const char* line3, const char* line4);
void loadConfigurations();
void setupServerRoutes();
void printAccessInfo();
void connectToWiFi();
void startAsyncWiFiConnection();
void saveScheduleConfig();
void handleAutoLDRMode();
void handleScheduledMode();
void ensureAPActive();
void maintainAPConnection();

void setup() {
  Serial.begin(115200);
  // Start immediately

  // Initialize SPIFFS for saved networks storage
  if (!SPIFFS.begin(true)) {
    SPIFFS.format();
    SPIFFS.begin(true);
  }

  // Initialize hardware
  initializeHardware();
  
  // Initialize display
  initializeDisplay();
  
  // Setup WiFi and Access Point
  initializeNetwork();
  
  // Load saved configurations
  loadConfigurations();
  
  // Start async WiFi connection if configured
  startAsyncWiFiConnection();

  // Setup web server routes
  setupServerRoutes();
  server.begin();

  // Setup mDNS - quick attempt
  if (MDNS.begin("lux2g0")) {
    MDNS.addService("http", "tcp", 80);
  }

  // Configure NTP (non-blocking)
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  printAccessInfo();
}

void loop() {
  currentMillis = millis();

  // Handle web server requests immediately
  server.handleClient();
  
  // Update display
  updateDisplay();
  
  // Quick yield for ESP32 background tasks
  yield();
  
  // Check AP status less frequently
  static unsigned long lastAPCheck = 0;
  if (currentMillis - lastAPCheck > 60000) { // Every 60s instead of 30s
    lastAPCheck = currentMillis;
    ensureAPActive();
  }

  // Handle deferred WiFi connection
  if (needsWiFiReconnect) {
    needsWiFiReconnect = false;
    Serial.println("Deferred WiFi reconnect triggered");
    // No delay - connect immediately
    connectToWiFi();
  }
  
  // Save configuration immediately (no delay)
  if (needsSaveConfig) {
    needsSaveConfig = false;
    saveScheduleConfig();
  }

  // Read LDR sensor every 1000ms (less frequent for better performance)
  if (currentMillis - lastLDRRead >= 1000) {
    lastLDRRead = currentMillis;
    ldrValue = analogRead(LDR_PIN);
  }

  // Control based on active mode
  switch (currentMode) {
    case MODE_MANUAL:
      // Manual mode - relays controlled by user
      break;
      
    case MODE_AUTO_LDR:
      handleAutoLDRMode();
      break;
      
    case MODE_SCHEDULED:
      handleScheduledMode();
      break;
  }

  // Check WiFi connectivity status periodically
  static unsigned long lastConnCheck = 0;
  if (currentMillis - lastConnCheck > 5000) {  // Check every 5 seconds
    lastConnCheck = currentMillis;
    
    if (!wifiConnected && WiFi.status() == WL_CONNECTED) {
      // Just connected!
      wifiConnected = true;
      Serial.printf("✓ WiFi Connected! IP: %s\n", WiFi.localIP().toString().c_str());
      
      // Show connection on display
      displayMessage("WiFi Connected!", 
                     WiFi.SSID().c_str(), 
                     "IP Address:",
                     WiFi.localIP().toString().c_str());
      
      // Configure NTP 
      configTime(gmtOffset_sec, daylightOffset_sec, "it.pool.ntp.org", "europe.pool.ntp.org", "time.google.com");
      wifiReconnectAttempts = 0;
      
    } else if (wifiConnected && WiFi.status() != WL_CONNECTED) {
      // Lost connection
      wifiConnected = false;
      ntpSynced = false;  // Lost Internet too
      WiFi.mode(WIFI_AP);  // Ensure AP stays active
    }
  }
  
  // Check NTP sync periodically but non-blocking
  static unsigned long lastNTPCheck = 0;
  if (wifiConnected && !ntpSynced && currentMillis - lastNTPCheck > 30000) {  // Every 30 seconds
    lastNTPCheck = currentMillis;
    
    // Quick non-blocking check
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 10)) {  // Only 10ms timeout
      ntpSynced = true;
    }
  }
  
  // Verify NTP is still working
  if (wifiConnected && ntpSynced && currentMillis - lastNTPCheck > 60000) {
    lastNTPCheck = currentMillis;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 100)) {
      ntpSynced = false;
    }
  }
  
  // Try WiFi reconnection but limit to 2 attempts total
  if (!wifiConnected && strlen(currentConfig.ssid) > 0 && wifiReconnectAttempts < 2) {
    if (currentMillis - lastReconnectAttempt >= 60000) {
      lastReconnectAttempt = currentMillis;
      wifiReconnectAttempts++;
      connectToWiFi();
      
      if (wifiConnected) {
        wifiReconnectAttempts = 0; // Reset on success
      }
    }
  }
  
  // Periodic AP check is now handled by ensureAPActive()
}