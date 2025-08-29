// G_Storage.ino - Preferences and storage management

// Save network to SPIFFS for quick recall
bool saveNetworkToSPIFFS(const char* ssid, const char* password, bool useStatic, 
                        IPAddress ip, IPAddress gw, IPAddress sn) {
  String filename = "/networks.json";
  
  JsonDocument doc;
  JsonArray networks;
  
  // Load existing networks if file exists
  if (SPIFFS.exists(filename)) {
    File file = SPIFFS.open(filename, "r");
    if (file) {
      DeserializationError error = deserializeJson(doc, file);
      file.close();
      if (error == DeserializationError::Ok) {
        networks = doc["networks"].as<JsonArray>();
      }
    }
  }
  
  // If no array exists, create one
  if (!networks) {
    networks = doc["networks"].to<JsonArray>();
  }
  
  // Check if network already exists and remove it
  for (int i = networks.size() - 1; i >= 0; i--) {
    if (networks[i]["ssid"] == ssid) {
      networks.remove(i);
    }
  }
  
  // Add new/updated network (max 10 networks)
  if (networks.size() >= 10) {
    networks.remove(0); // Remove oldest
  }
  
  JsonObject network = networks.add<JsonObject>();
  network["ssid"] = ssid;
  network["password"] = password;
  network["useStaticIP"] = useStatic;
  if (useStatic) {
    network["ip"] = ip.toString();
    network["gateway"] = gw.toString();
    network["subnet"] = sn.toString();
  }
  network["lastUsed"] = millis();
  
  // Save to file
  File file = SPIFFS.open(filename, "w");
  if (!file) {
    // Failed to save
    return false;
  }
  
  serializeJson(doc, file);
  file.close();
  
  // Network saved
  return true;
}

// Get all saved networks as JSON
String getSavedNetworksJSON() {
  JsonDocument doc;
  JsonArray networks = doc["networks"].to<JsonArray>();
  
  String filename = "/networks.json";
  if (SPIFFS.exists(filename)) {
    File file = SPIFFS.open(filename, "r");
    if (file) {
      JsonDocument savedDoc;
      if (deserializeJson(savedDoc, file) == DeserializationError::Ok) {
        JsonArray saved = savedDoc["networks"];
        for (JsonVariant v : saved) {
          JsonObject network = networks.add<JsonObject>();
          network["ssid"] = v["ssid"];
          network["hasPassword"] = strlen(v["password"].as<const char*>()) > 0;
          network["useStaticIP"] = v["useStaticIP"];
          if (v["useStaticIP"]) {
            network["ip"] = v["ip"];
          }
        }
      }
      file.close();
    }
  }
  
  String output;
  serializeJson(doc, output);
  return output;
}

// Load specific network from SPIFFS
bool loadNetworkFromSPIFFS(const char* ssid, WiFiConfig& config) {
  String filename = "/networks.json";
  
  if (!SPIFFS.exists(filename)) {
    return false;
  }
  
  File file = SPIFFS.open(filename, "r");
  if (!file) {
    return false;
  }
  
  JsonDocument doc;
  if (deserializeJson(doc, file) != DeserializationError::Ok) {
    file.close();
    return false;
  }
  file.close();
  
  JsonArray networks = doc["networks"];
  for (JsonVariant v : networks) {
    if (v["ssid"] == ssid) {
      strlcpy(config.ssid, v["ssid"] | "", sizeof(config.ssid));
      strlcpy(config.password, v["password"] | "", sizeof(config.password));
      config.useStaticIP = v["useStaticIP"] | false;
      
      if (config.useStaticIP) {
        config.staticIP.fromString(v["ip"] | "");
        config.gateway.fromString(v["gateway"] | "");
        config.subnet.fromString(v["subnet"] | "255.255.255.0");
        config.dns1 = IPAddress(8,8,8,8);
        config.dns2 = IPAddress(8,8,4,4);
      }
      
      // Network loaded
      return true;
    }
  }
  
  return false;
}

// Delete network from SPIFFS
bool deleteNetworkFromSPIFFS(const char* ssid) {
  String filename = "/networks.json";
  
  if (!SPIFFS.exists(filename)) {
    return false;
  }
  
  File file = SPIFFS.open(filename, "r");
  if (!file) {
    return false;
  }
  
  JsonDocument doc;
  if (deserializeJson(doc, file) != DeserializationError::Ok) {
    file.close();
    return false;
  }
  file.close();
  
  JsonArray networks = doc["networks"];
  bool found = false;
  
  for (int i = networks.size() - 1; i >= 0; i--) {
    if (networks[i]["ssid"] == ssid) {
      networks.remove(i);
      found = true;
    }
  }
  
  if (found) {
    File file = SPIFFS.open(filename, "w");
    if (file) {
      serializeJson(doc, file);
      file.close();
      // Network deleted
      return true;
    }
  }
  
  return false;
}

void loadConfigurations() {
  // Initialize Preferences
  preferences.begin("wifi-config", false);
  loadWiFiConfig();
  loadScheduleConfig();
}

void loadWiFiConfig() {
  // Load WiFi configuration
  preferences.getString("ssid", currentConfig.ssid, sizeof(currentConfig.ssid));
  preferences.getString("password", currentConfig.password, sizeof(currentConfig.password));
  currentConfig.useStaticIP = preferences.getBool("useStaticIP", false);
  
  if (currentConfig.useStaticIP) {
    uint32_t ip = preferences.getUInt("staticIP", 0);
    uint32_t gw = preferences.getUInt("gateway", 0);
    uint32_t sn = preferences.getUInt("subnet", 0xFFFFFF00); // 255.255.255.0
    uint32_t d1 = preferences.getUInt("dns1", 0x08080808);   // 8.8.8.8
    uint32_t d2 = preferences.getUInt("dns2", 0x08080404);   // 8.8.4.4
    
    currentConfig.staticIP = IPAddress(ip);
    currentConfig.gateway = IPAddress(gw);
    currentConfig.subnet = IPAddress(sn);
    currentConfig.dns1 = IPAddress(d1);
    currentConfig.dns2 = IPAddress(d2);
  }
  
  if (strlen(currentConfig.ssid) > 0) {
    Serial.printf("WiFi config loaded: SSID=%s, Pass=***\n", currentConfig.ssid);
    // Connection will be handled asynchronously in setup()
  } else {
    Serial.println("No saved WiFi configuration");
  }
}

void saveWiFiConfig() {
  Serial.printf("Saving WiFi config: SSID=%s\n", currentConfig.ssid);
  preferences.putString("ssid", currentConfig.ssid);
  preferences.putString("password", currentConfig.password);
  preferences.putBool("useStaticIP", currentConfig.useStaticIP);
  
  if (currentConfig.useStaticIP) {
    preferences.putUInt("staticIP", (uint32_t)currentConfig.staticIP);
    preferences.putUInt("gateway", (uint32_t)currentConfig.gateway);
    preferences.putUInt("subnet", (uint32_t)currentConfig.subnet);
    preferences.putUInt("dns1", (uint32_t)currentConfig.dns1);
    preferences.putUInt("dns2", (uint32_t)currentConfig.dns2);
  }
  
  // Also save to SPIFFS for quick recall
  saveNetworkToSPIFFS(currentConfig.ssid, currentConfig.password, 
                     currentConfig.useStaticIP, currentConfig.staticIP, 
                     currentConfig.gateway, currentConfig.subnet);
  
  // WiFi config saved
}

void loadScheduleConfig() {
  // Load operation mode
  currentMode = (OperationMode)preferences.getInt("mode", MODE_MANUAL);
  
  // Load LDR threshold
  ldrThreshold = preferences.getInt("ldrThreshold", 500);
  
  // Load schedule
  schedule.onHour = preferences.getInt("onHour", 8);
  schedule.onMinute = preferences.getInt("onMinute", 0);
  schedule.onSecond = preferences.getInt("onSecond", 0);
  schedule.offHour = preferences.getInt("offHour", 20);
  schedule.offMinute = preferences.getInt("offMinute", 0);
  schedule.offSecond = preferences.getInt("offSecond", 0);
  
  // Configuration loaded
}

void saveScheduleConfig() {
  preferences.putInt("mode", (int)currentMode);
  preferences.putInt("ldrThreshold", ldrThreshold);
  preferences.putInt("onHour", schedule.onHour);
  preferences.putInt("onMinute", schedule.onMinute);
  preferences.putInt("onSecond", schedule.onSecond);
  preferences.putInt("offHour", schedule.offHour);
  preferences.putInt("offMinute", schedule.offMinute);
  preferences.putInt("offSecond", schedule.offSecond);
  
  // Configuration saved
}