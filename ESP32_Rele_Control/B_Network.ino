// B_Network.ino - WiFi and Access Point management

void initializeNetwork() {
  // Quick reset
  WiFi.persistent(false);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  delay(50); // Reduced delay
  
  // Setup Access Point
  setupAccessPoint();
}

void setupAccessPoint() {
  // Set the AP name
  strcpy(ap_ssid, "lux2g0-AP");
  
  // Quick WiFi reset
  WiFi.persistent(false);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  delay(50); // Reduced delay
  
  // Configure IP settings BEFORE setting mode
  IPAddress local_IP(192, 168, 4, 1);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);
  
  // Set AP mode
  WiFi.mode(WIFI_AP);
  delay(50); // Reduced delay
  
  // Configure AP network settings
  WiFi.softAPConfig(local_IP, gateway, subnet);
  
  // Create Access Point with password
  // Using channel 1 for maximum compatibility with all devices
  // No connection limit, hidden SSID disabled (0), max connections 8
  bool apSuccess = WiFi.softAP(ap_ssid, ap_password, 1, 0, 8);
  
  if (!apSuccess) {
    delay(100);
    // Try again with absolute minimum config
    apSuccess = WiFi.softAP(ap_ssid, ap_password, 1);
    
    if (!apSuccess) {
      WiFi.softAP("ESP32-EMERGENCY");  // Open network as last resort
    }
  }
  
  // Disable power saving for stability
  esp_wifi_set_ps(WIFI_PS_NONE);
  
  // Set AP bandwidth to 20MHz for better compatibility (not 40MHz)
  esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_BW_HT20);
  
  // Set max TX power for better range
  esp_wifi_set_max_tx_power(84); // 84 = 21dBm (max allowed in most regions)
  
  // Wait for AP to initialize
  delay(200); // Reduced delay
  
  // Update our SSID variable with actual name if different
  String actualSSID = WiFi.softAPSSID();
  if (actualSSID != String(ap_ssid)) {
    strcpy(ap_ssid, actualSSID.c_str());
  }
  
  printAccessInfo();
}

void connectToWiFi() {
  if (strlen(currentConfig.ssid) == 0) {
    Serial.println("No SSID configured, skipping connection");
    return;
  }
  
  Serial.printf("Connecting to WiFi: %s\n", currentConfig.ssid);
  
  // Switch to STA+AP mode
  WiFi.mode(WIFI_AP_STA);
  
  // Configure station
  if (currentConfig.useStaticIP && currentConfig.staticIP != IPAddress(0,0,0,0)) {
    WiFi.config(currentConfig.staticIP, currentConfig.gateway, 
                currentConfig.subnet, currentConfig.dns1, currentConfig.dns2);
  }
  
  WiFi.begin(currentConfig.ssid, currentConfig.password);
  
  // Quick connection attempt (2 seconds max for immediate feedback)
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 4) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.printf("✓ Connected! IP: %s\n", WiFi.localIP().toString().c_str());
    
    // Show connection on display
    displayMessage("WiFi Connected!", 
                   WiFi.SSID().c_str(), 
                   "IP Address:",
                   WiFi.localIP().toString().c_str());
    
    // IMPORTANT: Keep both AP and STA active
    WiFi.mode(WIFI_AP_STA);
    
    // Disable power saving for better connectivity
    esp_wifi_set_ps(WIFI_PS_NONE);
    
    // Configure NTP but don't wait - will sync in background
    configTime(gmtOffset_sec, daylightOffset_sec, "it.pool.ntp.org", "europe.pool.ntp.org", "time.google.com");
    
    // Don't block here - NTP will sync automatically in background
    ntpSynced = false;
    
    // Reset reconnect attempts on success
    wifiReconnectAttempts = 0;
    
  } else {
    wifiConnected = false;
    Serial.printf("✗ Connection failed (Status: %d)\n", WiFi.status());
    // Keep AP active
    WiFi.mode(WIFI_AP);
    
    // Show failure on display
    displayMessage("WiFi Failed",
                   "Check password",
                   currentConfig.ssid,
                   nullptr);
  }
}

void printAccessInfo() {
  Serial.println("\n╔════════════════════════════════════╗");
  Serial.println("║         ACCESS POINT READY         ║");
  Serial.println("╠════════════════════════════════════╣");
  Serial.printf("║ Network: %-26s║\n", ap_ssid);
  if (strlen(ap_password) > 0) {
    Serial.printf("║ Password: %-25s║\n", ap_password);
  } else {
    Serial.println("║ Password: OPEN (no password)      ║");
  }
  Serial.println("║ URL: http://192.168.4.1           ║");
  
  if (wifiConnected) {
    Serial.println("╠════════════════════════════════════╣");
    Serial.println("║ Also connected to WiFi:            ║");
    Serial.printf("║ %s\n", WiFi.SSID().c_str());
    Serial.printf("║ Local: http://%-22s║\n", WiFi.localIP().toString().c_str());
  }
  
  Serial.println("╚════════════════════════════════════╝\n");
}

void ensureAPActive() {
  // This function is now called from loop every 60 seconds
  // No need for internal timing
    
  wifi_mode_t mode;
  esp_wifi_get_mode(&mode);
  
  // Only restart AP if it's completely off
  if (mode == WIFI_MODE_NULL) {
    setupAccessPoint();
  } else if (mode == WIFI_MODE_STA) {
    // If only STA mode, switch to AP_STA
    WiFi.mode(WIFI_AP_STA);
  }
  
  // Keep power saving disabled for better performance
  if (WiFi.softAPgetStationNum() == 0 && wifiConnected) {
    esp_wifi_set_ps(WIFI_PS_NONE);
  }
}

void maintainAPConnection() {
  // Keep AP active
  esp_wifi_set_ps(WIFI_PS_NONE);
}

void startAsyncWiFiConnection() {
  // Start WiFi connection asynchronously without blocking
  if (strlen(currentConfig.ssid) == 0) {
    return;
  }
  
  Serial.printf("Starting async WiFi connection to: %s\n", currentConfig.ssid);
  
  // Show connecting status on display
  displayMessage("Connecting WiFi...",
                 currentConfig.ssid,
                 "Please wait",
                 nullptr);
  
  // Ensure we're in AP+STA mode
  WiFi.mode(WIFI_AP_STA);
  
  // Configure station
  if (currentConfig.useStaticIP && currentConfig.staticIP != IPAddress(0,0,0,0)) {
    WiFi.config(currentConfig.staticIP, currentConfig.gateway, 
                currentConfig.subnet, currentConfig.dns1, currentConfig.dns2);
  }
  
  // Start connection - non-blocking
  WiFi.begin(currentConfig.ssid, currentConfig.password);
  
  // Connection will be checked in loop()
}