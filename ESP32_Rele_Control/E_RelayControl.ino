// E_RelayControl.ino - Relay control logic

void initializeHardware() {
  // Configure pins
  pinMode(RELE1_PIN, OUTPUT);
  #ifdef USE_RELAY2
    pinMode(RELE2_PIN, OUTPUT);
  #endif
  // GPIO34 is input only with internal pull-down, no pinMode needed
  
  // Relays initially off (active low)
  digitalWrite(RELE1_PIN, HIGH);
  #ifdef USE_RELAY2
    digitalWrite(RELE2_PIN, HIGH);
  #endif
}

void setRelay(int relay, bool state) {
  if (relay == 1) {
    rele1State = state;
    digitalWrite(RELE1_PIN, state ? LOW : HIGH); // Active low
  }
  #ifdef USE_RELAY2
  else if (relay == 2) {
    rele2State = state;
    digitalWrite(RELE2_PIN, state ? LOW : HIGH); // Active low
  }
  #endif
  
  // Force immediate display update
  updateDisplay();
}

void handleAutoLDRMode() {
  // Read LDR value (already updated in loop)
  // LDR logic: more light = lower value
  // Dark = high value (near 4095)
  // Light = low value (near 0)
  
  if (ldrValue > ldrThreshold) {
    // Dark - turn on relays
    if (!rele1State) {
      setRelay(1, true);
    }
    #ifdef USE_RELAY2
    if (!rele2State) {
      setRelay(2, true);
    }
    #endif
  } else {
    // Light - turn off relays
    if (rele1State) {
      setRelay(1, false);
    }
    #ifdef USE_RELAY2
    if (rele2State) {
      setRelay(2, false);
    }
    #endif
  }
}

void handleScheduledMode() {
  // Check time every 1 second for faster response
  if (currentMillis - lastTimeCheck < 1000) return;
  lastTimeCheck = currentMillis;
  
  // Quick check for NTP sync if WiFi connected
  if (wifiConnected && !ntpSynced) {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 10)) {  // Super quick 10ms check
      ntpSynced = true;
    }
  }
  
  // If no NTP sync yet, try to work with default time or skip
  if (!ntpSynced) {
    // If WiFi not connected, can't sync
    if (!wifiConnected) {
      return;
    }
    // Still waiting for NTP but continue checking
    return;
  }
  
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return;
  }
  
  // Convert current time to seconds since midnight
  int currentSeconds = timeinfo.tm_hour * 3600 + 
                      timeinfo.tm_min * 60 + 
                      timeinfo.tm_sec;
  
  // Convert schedule times to seconds since midnight
  int onSeconds = schedule.onHour * 3600 + 
                 schedule.onMinute * 60 + 
                 schedule.onSecond;
  int offSeconds = schedule.offHour * 3600 + 
                  schedule.offMinute * 60 + 
                  schedule.offSecond;
  
  bool shouldBeOn = false;
  
  if (onSeconds < offSeconds) {
    // Normal case: on during day
    shouldBeOn = (currentSeconds >= onSeconds && currentSeconds < offSeconds);
  } else {
    // Inverted: on during night
    shouldBeOn = (currentSeconds >= onSeconds || currentSeconds < offSeconds);
  }
  
  // Update relays if needed
  if (shouldBeOn && !rele1State) {
    setRelay(1, true);
    #ifdef USE_RELAY2
    setRelay(2, true);
    #endif
  } else if (!shouldBeOn && rele1State) {
    setRelay(1, false);
    #ifdef USE_RELAY2
    setRelay(2, false);
    #endif
  }
}