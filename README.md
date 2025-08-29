# lux2g0 - ESP32 Smart WiFi Relay Control System

![Version](https://img.shields.io/badge/version-1.2.0-blue)
![License](https://img.shields.io/badge/license-GPL%20v3-green)
![Platform](https://img.shields.io/badge/platform-ESP32-orange)

Sistema di controllo relè WiFi intelligente basato su ESP32 con interfaccia web moderna, sensore di luminosità e programmazione oraria NTP.

## 🌟 Caratteristiche Principali

- **Single relay control** ottimizzato
- **OTA firmware update** via interfaccia web
- **WiFi dual mode** (AP + STA simultanei)
- **Light sensor automation** (LDR)
- **Time-based scheduling** (NTP sync)
- **OLED display 1.3"** con animazioni
- **Dark/Light theme** con persistenza
- **Collapsible sections** interfaccia
- **RESTful API** endpoints
- **mDNS support** (lux2g0.local)

## 📋 Hardware Richiesto

- ESP32 DevKit V1 o compatibile
- Modulo relè 1 canale (5V) o compatibile
- Sensore LDR con modulo comparatore (3 pin)
- Display OLED 1.3" I2C SSD1306 (128x64) - Opzionale
- Alimentatore USB 5V (minimo 1A)
- Cavi jumper per collegamenti

## 🔌 Schema Collegamenti

### Modulo Relè
```
ESP32          Relè
-----          ----
5V       →     VCC
GND      →     GND
GPIO26   →     IN
```

### Sensore LDR
```
ESP32          Modulo LDR
-----          ----------
3.3V     →     VCC (IMPORTANTE: 3.3V, non 5V!)
GND      →     GND
GPIO34   →     AO (Analog Output)
```

### Display OLED 1.3" (Opzionale)
```
ESP32          Display OLED
-----          ------------
3.3V     →     VCC
GND      →     GND
GPIO21   →     SDA
GPIO22   →     SCL
```

## 💻 Installazione Software

### 1. Configura Arduino IDE

Aggiungi questo URL in **File → Preferences → Additional Board Manager URLs**:
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

### 2. Installa Board Support

**Tools → Board → Board Manager**:
- Cerca "**esp32**"
- Installa "**esp32 by Espressif Systems**"

### 3. Installa Librerie Richieste

**Library Manager** → Installa:
- **ArduinoJson** di Benoit Blanchon (v6.x o superiore)
- **Adafruit SSD1306** (per display OLED 1.3")
- **Adafruit GFX Library** (dipendenza per SSD1306)

### 4. Configurazione Board

**Tools** → Seleziona:
- Board: **ESP32 Dev Module**
- Upload Speed: **115200**
- Flash Frequency: **80MHz**
- Flash Mode: **QIO**
- Flash Size: **4MB (32Mb)**
- Partition Scheme: **Default 4MB**

### 5. Upload del Codice

1. Apri `ESP32_Rele_Control/ESP32_Rele_Control.ino`
2. Seleziona la porta COM corretta
3. Click **Upload** (→)

## 🚀 Primo Utilizzo

1. **Alimenta l'ESP32** e apri il Serial Monitor (115200 baud)

2. **Connetti al WiFi Access Point**:
   - SSID: `lux2g0-XXXXXX` (XXXXXX = ultimi 6 caratteri del MAC)
   - Password: `12345678`

3. **Accedi all'interfaccia web**:
   - Apri browser: `http://192.168.4.1`

4. **Configura la tua rete WiFi** (opzionale):
   - Settings → inserisci SSID e password della tua rete

## 💡 Modalità Operative

### Modalità Manuale
Controllo diretto dei relè tramite interfaccia web. I relè mantengono lo stato anche dopo riavvio.

### Modalità LDR (Light Dependent Resistor)
Controllo automatico basato sulla luminosità ambientale:
- Sensore analogico su GPIO34 (lettura 0-4095)
- Soglia regolabile via interfaccia web
- Logica: Buio (valore < soglia) → Relè ON
- Anti-flickering con isteresi del 5%

### Modalità Temporizzata
Programmazione oraria con sincronizzazione NTP:
- Richiede connessione WiFi per sincronizzazione orario
- Supporta programmazioni a cavallo della mezzanotte
- Precisione al secondo
- Fuso orario configurabile (default: GMT+1 Roma)

## 🌐 Modalità di Accesso

### Access Point Mode (sempre attivo)
- SSID: `lux2g0-XXXXXX`
- Password: `12345678`
- IP: `192.168.4.1`

### Station Mode (dopo configurazione WiFi)
- IP locale assegnato dal router (visibile nel Serial Monitor)
- mDNS: `http://lux2g0.local` (se supportato dalla rete)

## 🔧 API RESTful

### Endpoints Disponibili

```javascript
// Status sistema
GET /api/status

// Controllo relè
POST /api/control
Body: {"rele": 1, "state": true}  // rele: sempre 1

// Cambio modalità
POST /api/auto
Body: {"mode": 0}  // 0=Manual, 1=LDR, 2=Scheduled

// Configurazione soglia LDR
POST /api/threshold
Body: {"threshold": 500}  // 0-4095

// Configurazione orari
POST /api/schedule
Body: {
  "onHour": 8, "onMinute": 0, "onSecond": 0,
  "offHour": 20, "offMinute": 30, "offSecond": 0
}

// Scansione reti WiFi
GET /api/scan

// Configurazione WiFi
POST /api/wifi/config
Body: {"ssid": "MyNetwork", "password": "MyPassword"}
```

### Esempio JavaScript
```javascript
// Accendi relè 1
fetch('/api/control', {
  method: 'POST',
  headers: {'Content-Type': 'application/json'},
  body: JSON.stringify({rele: 1, state: true})
});
```

## ⚙️ Configurazione Avanzata

### Modifica Password AP
Nel file `ESP32_Rele_Control.ino`:
```cpp
const char* ap_password = "12345678";  // Cambia qui
```

### Modifica Prefisso Nome AP
Nel file `B_Network.ino`:
```cpp
sprintf(ap_ssid, "lux2g0-%06X", chipId);  // Cambia "lux2g0"
```

### Configurazione NTP
Nel file principale:
```cpp
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;      // GMT+1
const int daylightOffset_sec = 3600;  // Ora legale
```

## 🔒 Sicurezza

⚠️ **ATTENZIONE ALTA TENSIONE**
- Scollega sempre l'alimentazione prima di modificare i collegamenti
- Usa cavi adeguati per la corrente prevista
- Isola tutti i collegamenti esposti
- Il modulo relè deve essere certificato per il carico previsto

### Raccomandazioni Software
- Cambia la password AP di default
- Usa password WiFi sicure (WPA2)
- Non esporre il dispositivo direttamente su Internet
- Aggiorna regolarmente il firmware

## 🐛 Troubleshooting

### Il dispositivo non si connette al WiFi
- Verifica che il router usi 2.4GHz (ESP32 non supporta 5GHz)
- Controlla SSID e password (case sensitive)
- Riavvia dopo la configurazione

### I relè non commutano
- Verifica alimentazione 5V al modulo relè
- Controlla collegamenti GPIO26/GPIO27
- Verifica che i relè siano active-low (jumper su modulo)

### Lettura LDR instabile
- Usa alimentazione 3.3V per il sensore (non 5V!)
- Aggiungi condensatore 100nF su alimentazione sensore
- Regola soglia e isteresi dall'interfaccia

### Orario non sincronizzato
- Richiede connessione Internet attiva
- Verifica configurazione server NTP
- Attendi 30 secondi per prima sincronizzazione

## 📝 Struttura del Progetto

```
ESP32_Rele_Control/
├── ESP32_Rele_Control.ino    # File principale
├── A_Config.ino               # Configurazioni e variabili globali
├── B_Network.ino              # Gestione WiFi e connettività
├── C_WebServer.ino            # API REST e routing
├── D_WebPages.ino             # Interfaccia web HTML/CSS/JS
├── E_RelayControl.ino         # Logica controllo relè e modalità
├── F_Display.ino              # Gestione display OLED 1.3"
└── G_Storage.ino              # Persistenza dati (Preferences)
```

## 🆕 Ultime Modifiche (v1.2.0)

### 🔄 OTA Firmware Update
- **Upload firmware via web**: Caricamento file .bin direttamente dall'interfaccia
- **Barra progresso avanzata**: 40px altezza con percentuale integrata
- **Countdown riavvio**: Timer 10 secondi con refresh automatico pagina
- **Simulazione progresso**: Animazione fluida durante upload
- **Gestione errori migliorata**: Feedback visivo immediato per errori

### 🎨 Interfaccia Utente Rinnovata
- **Sezioni collassabili**: WiFi Configuration nascosta di default
- **Riorganizzazione layout**: Saved Networks prima di WiFi Configuration
- **File input personalizzato**: Stile coerente per selezione firmware
- **Nuovo schema colori**: Rosso spento (#B84444) invece di arancione
- **Pulsanti migliorati**: Icone integrate e feedback visivo

### 📟 Display OLED Miglioramenti
- **Scorrimento WiFi**: Animazione quando si passa da AP a STA
- **Modalità LDR ottimizzata**: "Mode: LDR" → "Threshold: xxx"
- **Display intelligente data/uptime**: 
  - Data DD-MM-YYYY quando connesso WiFi
  - Uptime HH:MM:SS quando solo AP
- **Intervallo scorrimento**: 5 secondi per migliore leggibilità
- **Fix sintassi**: Risolti errori parentesi graffe

### 🛠️ Ottimizzazioni Tecniche
- **HTML inline**: Servizio diretto senza SPIFFS per velocità
- **Gestione memoria**: Ottimizzazione uso RAM
- **Compatibilità ESP32 Core**: Supporto versioni 2.0.x e 3.x
- **Fix compilazione**: Risolti errori linking Arduino IDE

### Versioni Precedenti

#### v1.1.1
- **Modalità singolo relè**: Rimosso supporto secondo relè per semplificazione
- **Connessione WiFi asincrona**: Avvio immediato senza blocchi
- **Display ottimizzato**: Testi in grassetto per i valori dopo i due punti
- **Layout display uniforme**: Font 7x13 per tutto il testo

## 📄 Licenza

GNU GPL v3 - Software libero con copyleft

## 👨‍💻 Autore

marcuzz0

---

**Note**: Sviluppato e testato con Arduino IDE 2.x e ESP32 Core 2.x