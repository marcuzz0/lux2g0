// A_Config.ino - Configuration and global variables
// Includes and declarations are in the main ESP32_Rele_Control.ino file

// RELAY CONFIGURATION
// #define USE_RELAY2  // Comment this line to disable relay 2 (use only relay 1)

// Access Point configuration
char ap_ssid[32];
const char* ap_password = "12345678";  // Minimum 8 chars for WPA2

// NTP Configuration
const char* ntpServer = "it.pool.ntp.org";  // Italian NTP server
const long gmtOffset_sec = 3600;      // GMT+1 for Italy
const int daylightOffset_sec = 3600;  // +1 hour for DST

// Global variables
bool rele1State = false;
#ifdef USE_RELAY2
  bool rele2State = false;
#else
  // Define as constant false when not used
  const bool rele2State = false;
#endif
int ldrValue = 0;
int ldrThreshold = 500;
bool ntpSynced = false;
OperationMode currentMode = MODE_MANUAL;
TimeSchedule schedule;

// Timing variables
unsigned long currentMillis = 0;
unsigned long lastLDRRead = 0;
unsigned long lastTimeCheck = 0;

// Web server on port 80
WebServer server(80);
Preferences preferences;

// WiFi variables
WiFiConfig currentConfig;
bool wifiConnected = false;
bool needsWiFiReconnect = false;
bool needsSaveConfig = false;
unsigned long lastReconnectAttempt = 0;
unsigned long lastConfigSave = 0;
unsigned long lastStatusPrint = 0;
int wifiReconnectAttempts = 0;  // Count reconnection attempts

// Additional function declarations not in main file
void loadScheduleConfig();
void saveWiFiConfig();
void loadWiFiConfig();
void setupAccessPoint();
void setRelay(int relay, bool state);
void updateDisplay();
void displayMessage(const char* line1, const char* line2, 
                   const char* line3, const char* line4);

// Web handler functions
void handleRoot();
void handleSettings();
void handleAbout();
void handleControl();
void handleStatus();
void handleScanNetworks();
void handleDeepScan();
void handleGetWiFiConfig();
void handleSetWiFiConfig();
void handleConnectWiFi();
void handleDisconnectWiFi();
void handleAutoMode();
void handleThreshold();
void handleSchedule();
void handleTheme();
void handleNTPSync();
void handleGetSavedNetworks();
void handleLoadNetwork();
void handleDeleteNetwork();

// HTML generation functions
String getHomeHTML();
String getSettingsHTML();
String getAboutHTML();

// OTA Update functions
void handleOTAUpload();
void handleOTAUploadFile();

// Network storage functions  
bool saveNetworkToSPIFFS(const char* ssid, const char* password, bool useStatic, 
                        IPAddress ip, IPAddress gw, IPAddress sn);
String getSavedNetworksJSON();
bool loadNetworkFromSPIFFS(const char* ssid, WiFiConfig& config);
bool deleteNetworkFromSPIFFS(const char* ssid);