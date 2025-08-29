# Schema di Collegamento ESP32 con Modulo Relè

## ⚡ IMPORTANTE - SICUREZZA 220V
**ATTENZIONE**: Questo progetto utilizza tensione di rete 220V che può essere MORTALE!
- Scollega SEMPRE l'alimentazione prima di fare modifiche
- Usa cavi adeguati per alta tensione
- Isola tutti i collegamenti esposti
- Se non sei esperto, fatti aiutare da un elettricista qualificato

## 📌 Collegamenti ESP32 - Modulo Relè

### Alimentazione del modulo relè:
- **VCC** del modulo relè → **5V** dell'ESP32 (pin 5V)
- **GND** del modulo relè → **GND** dell'ESP32
- **JD-VCC** → Collegato a VCC (se presente il jumper, lascialo inserito)

### Segnali di controllo:
- **IN1** del modulo relè → **GPIO 26** dell'ESP32
- **IN2** del modulo relè → **GPIO 27** dell'ESP32

## 💡 Collegamenti Sensore LDR (Modulo Fotoresistore)

### Collegamento modulo LDR:
- **VCC** del modulo LDR → **3.3V** dell'ESP32 (IMPORTANTE: 3.3V, non 5V!)
- **GND** del modulo LDR → **GND** dell'ESP32
- **DO** (Digital Output) → Non collegato (non utilizzato)
- **AO** (Analog Output) → **GPIO 34** dell'ESP32 (ADC1_CH6)

### Note sul sensore LDR:
- Il modulo ha un potenziometro per regolare la sensibilità (solo per DO)
- Utilizziamo l'uscita analogica AO per letture precise (0-4095)
- Valori bassi = buio, valori alti = luminoso
- La soglia è regolabile via web (default 500)

## 🔌 Collegamenti Lampada 220V

### Per il Relè 1 (Lampada):
1. **Fase (L) dalla presa 220V** → **COM** (comune) del Relè 1
2. **NO** (Normally Open) del Relè 1 → **Cavo verso la lampada** (fase)
3. **Neutro (N) dalla presa 220V** → **Direttamente alla lampada**
4. **Terra** (se presente) → **Collegare alla carcassa metallica della lampada**

### Schema visivo:
```
RETE 220V                    MODULO RELÈ                    LAMPADA
┌─────────┐                 ┌────────────┐                ┌─────────┐
│   L ────┼─────────────────┤ COM    NO  ├────────────────┤ Fase    │
│         │                 │            │                │         │
│   N ────┼─────────────────┼────────────┼────────────────┤ Neutro  │
│         │                 │            │                │         │
│  PE ────┼─────────────────┼────────────┼────────────────┤ Terra   │
└─────────┘                 └────────────┘                └─────────┘
                                  ↑
                            Controllo ESP32
                             (GPIO 26)
```

## 🔧 Note Importanti

1. **Logica del Relè**: 
   - Il codice è configurato per relè ATTIVI BASSI (LOW = ON, HIGH = OFF)
   - Se il tuo relè funziona al contrario, modifica nel codice:
     - `digitalWrite(RELE1_PIN, LOW)` → `digitalWrite(RELE1_PIN, HIGH)` per accendere
     - `digitalWrite(RELE1_PIN, HIGH)` → `digitalWrite(RELE1_PIN, LOW)` per spegnere

2. **Alimentazione ESP32**:
   - Usa un alimentatore USB 5V di buona qualità
   - Assicurati che possa fornire almeno 500mA

3. **Isolamento**:
   - Tieni separati i cavi 220V dai cavi a bassa tensione
   - Usa una scatola di derivazione per proteggere i collegamenti

## 📱 Configurazione WiFi

Nel file `ESP32_Rele_Control.ino`, modifica queste righe con i tuoi dati:
```cpp
// Riga 10-11
const char* sta_ssid = "TuaReteWiFi";      // Nome della tua rete WiFi
const char* sta_password = "TuaPassword";   // Password della tua rete WiFi
```

## 🚀 Utilizzo

1. **Carica il codice** sull'ESP32 usando Arduino IDE
2. **Apri il Serial Monitor** (115200 baud) per vedere gli IP
3. **Accedi al controllo** in due modi:
   - **Access Point**: Connetti al WiFi "ESP32_Rele_Control" (password: 12345678), poi vai a http://192.168.4.1
   - **Rete locale**: Vai all'IP mostrato nel Serial Monitor o http://esp32rele.local

## ⚠️ Troubleshooting

- **Il relè non si attiva**: Verifica che il modulo relè sia alimentato a 5V
- **Il relè funziona al contrario**: Inverti la logica HIGH/LOW nel codice
- **Non riesco a connettermi**: Verifica le credenziali WiFi nel codice
- **La pagina web non si carica**: Controlla l'IP nel Serial Monitor