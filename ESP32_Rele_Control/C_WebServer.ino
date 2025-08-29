// C_WebServer.ino - Web server routes and API handlers

void setupServerRoutes() {
  // Main pages
  server.on("/", handleRoot);
  server.on("/settings", handleSettings);
  server.on("/about", handleAbout);
  
  // Favicon handler (return empty to avoid 404)
  server.on("/favicon.ico", [](){
    server.send(204); // No Content
  });
  
  // API endpoints
  server.on("/api/control", handleControl);
  server.on("/api/status", handleStatus);
  server.on("/api/scan", handleScanNetworks);
  server.on("/api/wifi/config", HTTP_GET, handleGetWiFiConfig);
  server.on("/api/wifi/config", HTTP_POST, handleSetWiFiConfig);
  server.on("/api/wifi/connect", handleConnectWiFi);
  server.on("/api/wifi/disconnect", handleDisconnectWiFi);
  server.on("/api/wifi/saved", HTTP_GET, handleGetSavedNetworks);
  server.on("/api/wifi/load", HTTP_POST, handleLoadNetwork);
  server.on("/api/wifi/delete", HTTP_POST, handleDeleteNetwork);
  server.on("/api/auto", handleAutoMode);
  server.on("/api/threshold", handleThreshold);
  server.on("/api/schedule", handleSchedule);
  server.on("/api/theme", handleTheme);
  server.on("/api/ntp/sync", HTTP_POST, handleNTPSync);
  server.on("/api/ota/upload", HTTP_POST, handleOTAUpload, handleOTAUploadFile);
  
  Serial.println("Server routes configurate");
}

// Page handlers - serve inline HTML directly
void handleRoot() {
  // Set cache headers
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Connection", "close");
  
  // Serve inline HTML directly
  String html = getHomeHTML();
  server.send(200, "text/html", html);
}

void handleSettings() {
  // Set cache headers
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Connection", "close");
  
  // Serve inline HTML directly
  String html = getSettingsHTML();
  server.send(200, "text/html", html);
}

void handleAbout() {
  // Set cache headers
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Connection", "close");
  
  // Serve inline HTML directly
  String html = getAboutHTML();
  server.send(200, "text/html", html);
}

// API: Relay control
void handleControl() {
  // Add CORS headers
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  
  if (server.method() == HTTP_OPTIONS) {
    server.send(200);
    return;
  }
  
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"error\":\"Method not allowed\"}");
    return;
  }

  // Allow manual control only in MANUAL mode
  if (currentMode != MODE_MANUAL) {
    server.send(403, "application/json", "{\"error\":\"Manual control not allowed. Please switch to Manual mode first.\"}");
    return;
  }

  String body = server.arg("plain");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, body);

  if (error) {
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }

  int rele = doc["rele"];
  bool state = doc["state"];

  setRelay(rele, state);

  // Fast minimal response
  JsonDocument response;
  response["rele1"] = rele1State;
  response["rele2"] = rele2State;
  response["mode"] = currentMode;
  response["success"] = true;
  
  String output;
  serializeJson(response, output);
  server.send(200, "application/json", output);
}

// API: Status
void handleStatus() {
  // Add CORS headers
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  
  if (server.method() == HTTP_OPTIONS) {
    server.send(200);
    return;
  }
  
  // Minimal response for speed in AP mode
  JsonDocument doc;
  
  // Essential data always
  doc["rele1"] = rele1State;
  #ifdef USE_RELAY2
  doc["rele2"] = rele2State;
  #endif
  doc["mode"] = (int)currentMode;
  doc["wifiConnected"] = wifiConnected;
  doc["ntpSynced"] = ntpSynced;
  
  // Scheduled mode status - operational when WiFi is connected (NTP will auto-sync)
  doc["scheduledActive"] = wifiConnected;  // If WiFi connected, scheduled will work
  
  // Always show network info
  doc["apIP"] = WiFi.softAPIP().toString();
  doc["apSSID"] = ap_ssid;
  doc["apClients"] = WiFi.softAPgetStationNum();
  
  if (wifiConnected) {
    doc["localIP"] = WiFi.localIP().toString();
    doc["networkSSID"] = WiFi.SSID();
    doc["rssi"] = WiFi.RSSI();
  } else if (strlen(currentConfig.ssid) > 0) {
    doc["localIP"] = "Not connected";
    doc["networkSSID"] = String(currentConfig.ssid) + " (connecting...)";
  } else {
    doc["localIP"] = "Not configured";
    doc["networkSSID"] = "None";
  }
  
  // Only in LDR mode add LDR data
  if (currentMode == MODE_AUTO_LDR) {
    doc["ldrValue"] = ldrValue;
    doc["ldrThreshold"] = ldrThreshold;
    int percentuale = map(constrain(ldrValue, 0, 4095), 0, 4095, 100, 0);
    doc["ldrPercent"] = percentuale;
  }
  
  // Only in scheduled mode add time data
  if (currentMode == MODE_SCHEDULED) {
    if (ntpSynced) {
      struct tm timeinfo;
      if (getLocalTime(&timeinfo)) {
        char timeStr[9];
        sprintf(timeStr, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        doc["currentTime"] = timeStr;
      }
    }
    doc["schedule"]["onHour"] = schedule.onHour;
    doc["schedule"]["onMinute"] = schedule.onMinute;
    doc["schedule"]["onSecond"] = schedule.onSecond;
    doc["schedule"]["offHour"] = schedule.offHour;
    doc["schedule"]["offMinute"] = schedule.offMinute;
    doc["schedule"]["offSecond"] = schedule.offSecond;
  }

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// API: WiFi scan
void handleScanNetworks() {
  // Use dynamic allocation for unlimited networks
  JsonDocument doc;  // Dynamic allocation for 50+ networks
  JsonArray networks = doc["networks"].to<JsonArray>();
  
  // Ensure we're in AP_STA mode for scanning
  if (WiFi.getMode() != WIFI_AP_STA) {
    WiFi.mode(WIFI_AP_STA);
    delay(50); // Minimal delay
  }
  
  // Clear any previous scan
  WiFi.scanDelete();
  
  // Don't change mode - scan in current mode for stability
  WiFiMode_t currentMode = WiFi.getMode();
  if (currentMode == WIFI_OFF) {
    WiFi.mode(WIFI_STA);
    delay(100); // Reduced delay
  }
  
  // First attempt: Active scan with hidden networks
  int n = WiFi.scanNetworks(false, true, false, 300);
  
  // If we found very few networks, try passive scan for iPhone/mobile hotspots
  if (n < 5) {
    WiFi.scanDelete();
    delay(200); // Reduced delay
    
    // Second attempt: Passive scan (better for iPhone detection)
    n = WiFi.scanNetworks(false, true, true, 500);
  }
  
  // Final fallback
  if (n <= 0) {;
    WiFi.scanDelete();
    delay(500);
    n = WiFi.scanNetworks();
    Serial.printf("Basic scan found %d networks\n", n);
  }
  
  // Force check if scan is still running
  if (n == WIFI_SCAN_RUNNING) {
    Serial.println("Scan still running, waiting...");
    delay(2000);
    n = WiFi.scanComplete();
  }
  
  // Final attempt if needed
  if (n <= 0) {
    WiFi.mode(WIFI_STA);
    delay(50); // Reduced delay
    n = WiFi.scanNetworks(false, true);
    WiFi.mode(WIFI_AP_STA);
  }
  
  // If n is negative, it's an error code
  if (n < 0) {
    n = 0;
  }
  
  // Show ALL found networks - NO LIMIT
  int actualCount = 0;
  
  for (int i = 0; i < n; i++) {  // Remove the 50 limit
    String ssid = WiFi.SSID(i);
    
    // Skip completely invalid entries
    if (WiFi.RSSI(i) == 0) {
      continue;
    }
    
    // Handle empty SSIDs
    if (ssid.length() == 0) {
      ssid = "[Hidden Network]";
    }
    
    JsonObject network = networks.add<JsonObject>();
    network["ssid"] = ssid;
    network["rssi"] = WiFi.RSSI(i);
    network["secured"] = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
    network["channel"] = WiFi.channel(i);
    network["bssid"] = WiFi.BSSIDstr(i);
    
    actualCount++;
  }
  
  // Don't change mode back - stay in current mode
  
  // Clean up scan results
  WiFi.scanDelete();
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// API: Get WiFi config
void handleGetWiFiConfig() {
  JsonDocument doc;
  doc["ssid"] = currentConfig.ssid;
  doc["useStaticIP"] = currentConfig.useStaticIP;
  
  if (currentConfig.useStaticIP) {
    doc["ip"] = currentConfig.staticIP.toString();
    doc["gateway"] = currentConfig.gateway.toString();
    doc["subnet"] = currentConfig.subnet.toString();
    doc["dns1"] = currentConfig.dns1.toString();
    doc["dns2"] = currentConfig.dns2.toString();
  }
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// API: Set WiFi config
void handleSetWiFiConfig() {
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"error\":\"Method not allowed\"}");
    return;
  }
  
  String body = server.arg("plain");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, body);
  
  if (error) {
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }
  
  // Save configuration
  strlcpy(currentConfig.ssid, doc["ssid"] | "", sizeof(currentConfig.ssid));
  strlcpy(currentConfig.password, doc["password"] | "", sizeof(currentConfig.password));
  currentConfig.useStaticIP = doc["staticIP"] | false;
  
  if (currentConfig.useStaticIP) {
    String ip = doc["ip"] | "";
    String gw = doc["gateway"] | "";
    String sn = doc["subnet"] | "255.255.255.0";
    String d1 = doc["dns1"] | "8.8.8.8";
    String d2 = doc["dns2"] | "8.8.4.4";
    
    currentConfig.staticIP.fromString(ip);
    currentConfig.gateway.fromString(gw);
    currentConfig.subnet.fromString(sn);
    currentConfig.dns1.fromString(d1);
    currentConfig.dns2.fromString(d2);
  }
  
  Serial.printf("Saving WiFi config: SSID=%s\n", currentConfig.ssid);
  saveWiFiConfig();
  
  // Send response immediately before reconnecting
  JsonDocument response;
  response["success"] = true;
  response["message"] = "Configuration saved! Connecting...";
  String jsonResponse;
  serializeJson(response, jsonResponse);
  server.send(200, "application/json", jsonResponse);
  
  // Flag to reconnect in main loop
  needsWiFiReconnect = true;
}

// API: Connect WiFi
void handleConnectWiFi() {
  connectToWiFi();
  handleStatus();
}

// API: Disconnect WiFi
void handleDisconnectWiFi() {
  WiFi.disconnect(true);
  wifiConnected = false;
  handleStatus();
}

// API: Operation mode
void handleAutoMode() {
  // Add CORS headers
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  
  if (server.method() == HTTP_OPTIONS) {
    server.send(200);
    return;
  }
  
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"error\":\"Method not allowed\"}");
    return;
  }
  
  String body = server.arg("plain");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, body);
  
  if (error) {
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }
  
  int newMode = doc["mode"];
  if (newMode >= 0 && newMode <= 2) {
    // Allow all modes - NTP will sync automatically if needed
    
    // Immediate change without saving to flash
    currentMode = (OperationMode)newMode;
    
    // ALWAYS turn off relays when changing mode
    rele1State = false;
    #ifdef USE_RELAY2
    rele2State = false;
    #endif
    digitalWrite(RELE1_PIN, HIGH);  // Active low
    digitalWrite(RELE2_PIN, HIGH);  // Active low
    
    // If switching to scheduled mode and NTP is synced, check immediately
    if (newMode == 2 && ntpSynced) {
      struct tm timeinfo;
      if (getLocalTime(&timeinfo, 10)) {
        // Check if we should be ON now
        int currentSeconds = timeinfo.tm_hour * 3600 + timeinfo.tm_min * 60 + timeinfo.tm_sec;
        int onSeconds = schedule.onHour * 3600 + schedule.onMinute * 60;
        int offSeconds = schedule.offHour * 3600 + schedule.offMinute * 60;
        
        bool shouldBeOn = false;
        if (onSeconds < offSeconds) {
          shouldBeOn = (currentSeconds >= onSeconds && currentSeconds < offSeconds);
        } else {
          shouldBeOn = (currentSeconds >= onSeconds || currentSeconds < offSeconds);
        }
        
        if (shouldBeOn) {
          // Turn on immediately
          setRelay(1, true);
          setRelay(2, true);
          Serial.println("SCHEDULED: Immediate activation - in scheduled time");
        }
      }
    }
    
    // Flag to save later (in main loop)
    needsSaveConfig = true;
    lastConfigSave = millis();
    
    Serial.printf("Mode changed: %s\n",
                  currentMode == MODE_MANUAL ? "MANUAL" : 
                  currentMode == MODE_AUTO_LDR ? "AUTO LDR" : "SCHEDULED");
  }
  
  // Minimal fast response
  JsonDocument response;
  response["mode"] = currentMode;
  response["rele1"] = false;
  response["rele2"] = false;
  response["success"] = true;
  
  String output;
  serializeJson(response, output);
  server.send(200, "application/json", output);
}

// API: LDR threshold
void handleThreshold() {
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"error\":\"Method not allowed\"}");
    return;
  }
  
  String body = server.arg("plain");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, body);
  
  if (error) {
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }
  
  ldrThreshold = doc["threshold"];
  ldrThreshold = constrain(ldrThreshold, 0, 4095);
  Serial.printf("New LDR threshold: %d\n", ldrThreshold);
  saveScheduleConfig();
  
  handleStatus();
}

// API: Theme
void handleTheme() {
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"error\":\"Method not allowed\"}");
    return;
  }
  
  String body = server.arg("plain");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, body);
  
  if (!error) {
    String theme = doc["theme"] | "dark";
    preferences.putString("theme", theme);
    Serial.printf("Theme saved: %s\n", theme.c_str());
  }
  
  server.send(200, "application/json", "{\"success\":true}");
}

// API: Schedule
void handleSchedule() {
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"error\":\"Method not allowed\"}");
    return;
  }
  
  String body = server.arg("plain");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, body);
  
  if (error) {
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }
  
  if (doc["onHour"].is<int>()) schedule.onHour = doc["onHour"];
  if (doc["onMinute"].is<int>()) schedule.onMinute = doc["onMinute"];
  if (doc["onSecond"].is<int>()) schedule.onSecond = doc["onSecond"];
  if (doc["offHour"].is<int>()) schedule.offHour = doc["offHour"];
  if (doc["offMinute"].is<int>()) schedule.offMinute = doc["offMinute"];
  if (doc["offSecond"].is<int>()) schedule.offSecond = doc["offSecond"];
  
  // Validate times
  schedule.onHour = constrain(schedule.onHour, 0, 23);
  schedule.onMinute = constrain(schedule.onMinute, 0, 59);
  schedule.onSecond = constrain(schedule.onSecond, 0, 59);
  schedule.offHour = constrain(schedule.offHour, 0, 23);
  schedule.offMinute = constrain(schedule.offMinute, 0, 59);
  schedule.offSecond = constrain(schedule.offSecond, 0, 59);
  
  saveScheduleConfig();
  
  Serial.printf("New schedule: ON %02d:%02d:%02d, OFF %02d:%02d:%02d\n",
                schedule.onHour, schedule.onMinute, schedule.onSecond,
                schedule.offHour, schedule.offMinute, schedule.offSecond);
  
  handleStatus();
}

// API: Deep scan specifically for iPhone/mobile hotspots
void handleDeepScan() {
  Serial.println("\n=== DEEP SCAN FOR MOBILE HOTSPOTS ===");
  
  JsonDocument doc;  // Dynamic buffer for many networks
  JsonArray networks = doc["networks"].to<JsonArray>();
  
  // Temporarily switch to pure STA mode for better scanning
  wifi_mode_t originalMode;
  esp_wifi_get_mode(&originalMode);
  WiFi.mode(WIFI_STA);
  delay(200);
  
  // Multiple scan passes
  int maxNetworks = 0;
  
  for (int pass = 0; pass < 3; pass++) {
    Serial.printf("Deep scan pass %d...\n", pass + 1);
    WiFi.scanDelete();
    delay(100);
    
    // Alternate between scan types
    int n;
    if (pass == 0) {
      n = WiFi.scanNetworks(false, true, false, 300); // Active with hidden
    } else if (pass == 1) {
      n = WiFi.scanNetworks(false, true, true, 500); // Passive with hidden
    } else {
      n = WiFi.scanNetworks(true, true, false, 400); // Async with hidden
      delay(2000); // Wait for async scan
      n = WiFi.scanComplete();
    }
    
    if (n > maxNetworks) {
      maxNetworks = n;
      networks.clear(); // Clear and refill with better results
      
      for (int i = 0; i < n; i++) {
        String ssid = WiFi.SSID(i);
        if (ssid.length() == 0) ssid = "[Hidden]";
        
        JsonObject network = networks.add<JsonObject>();
        network["ssid"] = ssid;
        network["rssi"] = WiFi.RSSI(i);
        network["secured"] = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
        network["channel"] = WiFi.channel(i);
        network["bssid"] = WiFi.BSSIDstr(i);
        
        // Check for mobile hotspot patterns
        if (ssid.indexOf("iPhone") >= 0 || ssid.indexOf("iPad") >= 0 || 
            ssid.indexOf("Android") >= 0 || ssid.indexOf("'s ") >= 0) {
          network["mobile"] = true;
        }
      }
    }
    
    if (n >= 10) break; // If we found enough networks, stop
  }
  
  // Restore original WiFi mode
  WiFi.mode((WiFiMode_t)originalMode);
  
  doc["count"] = maxNetworks;
  Serial.printf("Deep scan complete: %d networks found\n", maxNetworks);
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// API: Force NTP sync
void handleNTPSync() {
  if (!wifiConnected) {
    server.send(400, "application/json", "{\"error\":\"No WiFi connection\"}");
    return;
  }
  
  Serial.println("Manual NTP sync requested...");
  
  // Try to sync NTP
  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov", "time.google.com");
  delay(1000);
  
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 5000)) {
    ntpSynced = true;
    
    JsonDocument doc;
    doc["success"] = true;
    doc["ntpSynced"] = true;
    doc["time"] = String(timeinfo.tm_hour) + ":" + String(timeinfo.tm_min) + ":" + String(timeinfo.tm_sec);
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
    
    Serial.printf("✓ NTP manually synced: %02d:%02d:%02d\n", 
                  timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  } else {
    ntpSynced = false;
    server.send(500, "application/json", "{\"error\":\"NTP sync failed\", \"ntpSynced\":false}");
    Serial.println("✗ Manual NTP sync failed");
  }
}

// API: Get saved networks
void handleGetSavedNetworks() {
  String response = getSavedNetworksJSON();
  server.send(200, "application/json", response);
}

// API: Load saved network
void handleLoadNetwork() {
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"error\":\"Method not allowed\"}");
    return;
  }
  
  String body = server.arg("plain");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, body);
  
  if (error) {
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }
  
  const char* ssid = doc["ssid"];
  if (!ssid) {
    server.send(400, "application/json", "{\"error\":\"SSID required\"}");
    return;
  }
  
  WiFiConfig tempConfig;
  if (loadNetworkFromSPIFFS(ssid, tempConfig)) {
    Serial.printf("Network loaded from SPIFFS: %s\n", ssid);
    // Copy to current config
    memcpy(&currentConfig, &tempConfig, sizeof(WiFiConfig));
    Serial.printf("Config copied: SSID=%s, hasPassword=%s\n", 
                  currentConfig.ssid, 
                  strlen(currentConfig.password) > 0 ? "yes" : "no");
    saveWiFiConfig();
    
    JsonDocument response;
    response["success"] = true;
    response["ssid"] = currentConfig.ssid;
    response["useStaticIP"] = currentConfig.useStaticIP;
    if (currentConfig.useStaticIP) {
      response["ip"] = currentConfig.staticIP.toString();
      response["gateway"] = currentConfig.gateway.toString();
      response["subnet"] = currentConfig.subnet.toString();
    }
    
    String jsonResponse;
    serializeJson(response, jsonResponse);
    server.send(200, "application/json", jsonResponse);
    
    // Start async connection immediately
    startAsyncWiFiConnection();
  } else {
    server.send(404, "application/json", "{\"error\":\"Network not found\"}");
  }
}

// API: Delete saved network
void handleDeleteNetwork() {
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"error\":\"Method not allowed\"}");
    return;
  }
  
  String body = server.arg("plain");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, body);
  
  if (error) {
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }
  
  const char* ssid = doc["ssid"];
  if (!ssid) {
    server.send(400, "application/json", "{\"error\":\"SSID required\"}");
    return;
  }
  
  bool deleted = deleteNetworkFromSPIFFS(ssid);
  
  JsonDocument response;
  response["success"] = deleted;
  response["message"] = deleted ? "Network deleted" : "Network not found";
  
  String jsonResponse;
  serializeJson(response, jsonResponse);
  server.send(200, "application/json", jsonResponse);
}

// OTA Firmware Upload Handler
void handleOTAUpload() {
  server.sendHeader("Connection", "close");
  server.send(200, "application/json", "{\"success\":true,\"message\":\"Upload completed, rebooting...\"}");
  delay(1000);
  ESP.restart();
}

// OTA File Upload Handler
void handleOTAUploadFile() {
  HTTPUpload& upload = server.upload();
  
  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("OTA Update Start: %s\n", upload.filename.c_str());
    
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
      Update.printError(Serial);
      return;
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
      return;
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) {
      Serial.printf("OTA Update Success: %uB\n", upload.totalSize);
    } else {
      Update.printError(Serial);
    }
  }
}