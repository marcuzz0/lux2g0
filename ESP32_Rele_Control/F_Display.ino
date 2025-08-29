// F_Display.ino - OLED Display management with U8g2
// Display OLED 1.3" I2C 
// Connections: SDA->GPIO21, SCL->GPIO22, VCC->3.3V, GND->GND

#ifdef USE_DISPLAY_U8G2
  #include <U8g2lib.h>
  #include <Wire.h>
  
  // Try different constructors for 1.3" 128x64 displays
  // Uncomment the one that works for your display:
  
  // Most common for 1.3" displays:
  U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
  
  // Alternative constructors to try if above doesn't work:
  // U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
  // U8G2_SSD1309_128X64_NONAME0_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
  // U8G2_SSD1309_128X64_NONAME2_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
  
  unsigned long lastDisplayUpdate = 0;
  const unsigned long DISPLAY_UPDATE_INTERVAL = 50; // Faster update for smooth scrolling
  
  // Scrolling text variables
  int scrollOffset = 0;
  unsigned long lastScrollUpdate = 0;
  const unsigned long SCROLL_INTERVAL = 50; // Smooth scroll every 50ms
  String currentText = "";
  String nextText = "";
  bool isScrolling = false;
  int scrollPosition = 128; // Start from right side of screen
  
  // Mode change tracking
  OperationMode lastMode = MODE_MANUAL;
  unsigned long modeChangeTime = 0;
  const unsigned long MODE_DISPLAY_TIME = 3000; // Show mode for 3 seconds after change
  
  // WiFi state tracking for scrolling
  bool lastWifiState = false;
  unsigned long wifiChangeTime = 0;
  const unsigned long WIFI_DISPLAY_TIME = 3000; // Show wifi state for 3 seconds after change
  
  bool displayConnected = false;
#else
  bool displayConnected = false;
  OperationMode lastMode = MODE_MANUAL;
  unsigned long modeChangeTime = 0;
#endif

void initializeDisplay() {
#ifdef USE_DISPLAY_U8G2
  Serial.println("Initializing U8g2 display...");
  
  // Initialize I2C
  Wire.begin(21, 22);  // SDA=GPIO21, SCL=GPIO22
  delay(100);
  
  // Initialize display
  display.begin();
  displayConnected = true;
  
  // Splash screen - centered lux2g0
  display.clearBuffer();
  
  // Use bigger font for splash
  display.setFont(u8g2_font_helvB12_tf);  // Helvetica Bold 12px
  
  // Center text calculation
  const char* text = "lux2g0";
  int textWidth = display.getStrWidth(text);
  int x = (128 - textWidth) / 2;  // Center horizontally
  int y = 35;  // Center vertically (64/2 + font_height/2)
  
  display.drawStr(x, y, text);
  display.sendBuffer();
  
  Serial.println("U8g2 display initialized!");
  delay(2500);  // Show splash for 2.5 seconds
#endif
}

void updateDisplay() {
#ifdef USE_DISPLAY_U8G2
  if (!displayConnected) return;
  
  if (millis() - lastDisplayUpdate < DISPLAY_UPDATE_INTERVAL) return;
  lastDisplayUpdate = millis();
  
  // Check if mode has changed
  if (currentMode != lastMode) {
    lastMode = currentMode;
    modeChangeTime = millis();
  }
  
  // Update alternate state every 2 seconds with 3 states for LDR/Scheduled modes
  static int alternateState = 0;
  static int targetState = 0;
  static unsigned long lastAlternate = 0;
  
  // Force display Mode for 3 seconds after mode change
  bool forceModeDisplay = (millis() - modeChangeTime < MODE_DISPLAY_TIME);
  
  if (forceModeDisplay) {
    // Reset to state 0 to show mode
    alternateState = 0;
    targetState = 0;
    lastAlternate = millis();
    isScrolling = false;
  } else if (millis() - lastAlternate > 2000 && !isScrolling) {
    lastAlternate = millis();
    // Set target state for scrolling
    if (currentMode == MODE_MANUAL) {
      // Manual mode doesn't alternate
      targetState = 0;
    } else if (currentMode == MODE_AUTO_LDR) {
      // LDR: alternate between LDR Value (0) and Threshold (1)
      targetState = (alternateState == 0) ? 1 : 0;
    } else if (currentMode == MODE_SCHEDULED) {
      // Scheduled: alternate between Scheduled (0) and Turn (1)
      targetState = (alternateState == 0) ? 1 : 0;
    }
    if (targetState != alternateState) {
      isScrolling = true;
      scrollPosition = 128;  // Start scroll from right
    }
  }
  
  // Handle scrolling animation
  if (isScrolling) {
    scrollPosition -= 4;  // Scroll speed
    if (scrollPosition <= 0) {
      scrollPosition = 0;
      alternateState = targetState;
      isScrolling = false;
    }
  }
  
  display.clearBuffer();
  display.setFont(u8g2_font_7x13_tf);  // Bigger font for better readability
  
  // Line 1: Mode with alternating info based on mode
  // Create a clipping region for line 1
  display.setClipWindow(0, 0, 128, 14);
  
  // Draw current text
  if (!isScrolling || scrollPosition > 0) {
    display.setCursor(isScrolling ? scrollPosition : 0, 13);
    switch(currentMode) {
      case MODE_MANUAL:
        display.setFont(u8g2_font_7x13B_tf);  // Bold for label
        display.print("Mode:");
        display.setFont(u8g2_font_7x13_tf);  // Normal for value
        display.print(" MANUAL");
        break;
    case MODE_AUTO_LDR:
      if (alternateState == 0) {
        // Show Mode: LDR
        display.setFont(u8g2_font_7x13B_tf);  // Bold for label
        display.print("Mode:");
        display.setFont(u8g2_font_7x13_tf);  // Normal for value
        display.print(" LDR");
      } else {
        // Show Threshold
        display.setFont(u8g2_font_7x13B_tf);  // Bold for label
        display.print("Threshold:");
        display.setFont(u8g2_font_7x13_tf);  // Normal for value
        display.print(" ");
        display.print(ldrThreshold);
      }
      break;
    case MODE_SCHEDULED:
      if (alternateState == 0) {
        // Show mode
        display.setFont(u8g2_font_7x13B_tf);  // Bold for label
        display.print("Mode:");
        display.setFont(u8g2_font_7x13_tf);  // Normal for value
        display.print(" SCHEDULED");
      } else {
        // Show Turn times
        display.setFont(u8g2_font_7x13B_tf);  // Bold for label
        display.print("Turn:");
        display.setFont(u8g2_font_7x13_tf);  // Normal for value
        display.print(" ");
        display.printf("%02d:%02d-%02d:%02d", 
                       schedule.onHour, schedule.onMinute,
                       schedule.offHour, schedule.offMinute);
      }
      break;
  }
  
  // Draw incoming text during scroll
  if (isScrolling && scrollPosition < 128) {
    display.setCursor(scrollPosition - 128, 13);
    switch(currentMode) {
      case MODE_MANUAL:
        display.setFont(u8g2_font_7x13B_tf);
        display.print("Mode:");
        display.setFont(u8g2_font_7x13_tf);
        display.print(" MANUAL");
        break;
      case MODE_AUTO_LDR:
        if (targetState == 0) {
          // Show Mode: LDR
          display.setFont(u8g2_font_7x13B_tf);
          display.print("Mode:");
          display.setFont(u8g2_font_7x13_tf);
          display.print(" LDR");
        } else {
          display.setFont(u8g2_font_7x13B_tf);
          display.print("Threshold:");
          display.setFont(u8g2_font_7x13_tf);
          display.print(" ");
          display.print(ldrThreshold);
        }
        break;
      case MODE_SCHEDULED:
        if (targetState == 0) {
          display.setFont(u8g2_font_7x13B_tf);
          display.print("Mode:");
          display.setFont(u8g2_font_7x13_tf);
          display.print(" SCHEDULED");
        } else {
          display.setFont(u8g2_font_7x13B_tf);
          display.print("Turn:");
          display.setFont(u8g2_font_7x13_tf);
          display.print(" ");
          display.printf("%02d:%02d-%02d:%02d", 
                         schedule.onHour, schedule.onMinute,
                         schedule.offHour, schedule.offMinute);
        }
        break;
    }
  }
  
  // Reset clipping window
  display.setMaxClipWindow();
  
  // Line 2: Show STA when connected, otherwise show AP
  display.setCursor(0, 28);
  if (wifiConnected) {
    // Show only STA when connected
    display.setFont(u8g2_font_7x13B_tf);  // Bold for label
    display.print("STA:");
    display.setFont(u8g2_font_7x13_tf);  // Normal for value
    display.print(" ");
    display.print(WiFi.localIP());
  } else {
    // Show AP when not connected
    display.setFont(u8g2_font_7x13B_tf);  // Bold for label
    display.print("AP:");
    display.setFont(u8g2_font_7x13_tf);  // Normal for value
    display.print(" 192.168.4.1");
  }
  
  // Line 3: Date when connected, Uptime when not
  display.setCursor(0, 43);
  
  if (wifiConnected) {
    // Show date when connected
    display.setFont(u8g2_font_7x13B_tf);  // Bold for label
    display.print("Date:");
    display.setFont(u8g2_font_7x13_tf);  // Normal for date
    display.print(" ");
    
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 10)) {
      display.printf("%02d-%02d-%04d", 
                     timeinfo.tm_mday, 
                     timeinfo.tm_mon + 1, 
                     timeinfo.tm_year + 1900);
    } else {
      display.print("--:--:----");
    }
  } else {
    // Show uptime when not connected
    display.setFont(u8g2_font_7x13B_tf);  // Bold for label
    display.print("Uptime:");
    display.setFont(u8g2_font_7x13_tf);  // Normal for time
    display.print(" ");
    
    unsigned long uptime = millis() / 1000;
    unsigned long hours = uptime / 3600;
    unsigned long minutes = (uptime % 3600) / 60;
    unsigned long seconds = uptime % 60;
    
    display.printf("%02lu:%02lu:%02lu", hours, minutes, seconds);
  }
  
  // Line 4: Relay status
  display.setCursor(0, 58);
  display.setFont(u8g2_font_7x13B_tf);  // Bold for label
  display.print("Relay:");
  display.setFont(u8g2_font_7x13_tf);  // Normal for status
  display.print(" ");
  display.print(rele1State ? "ON" : "OFF");
  
  display.sendBuffer();
#endif
}
}

void displayMessage(const char* line1, const char* line2, 
                   const char* line3, const char* line4) {
#ifdef USE_DISPLAY_U8G2
  if (!displayConnected) return;
  
  display.clearBuffer();
  display.setFont(u8g2_font_5x7_tf);  // Small clean font
  
  if (line1) {
    display.setCursor(0, 10);
    display.print(line1);
  }
  if (line2) {
    display.setCursor(0, 25);
    display.print(line2);
  }
  if (line3) {
    display.setCursor(0, 40);
    display.print(line3);
  }
  if (line4) {
    display.setCursor(0, 55);
    display.print(line4);
  }
  
  display.sendBuffer();
#endif
}