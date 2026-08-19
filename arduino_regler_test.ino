// ==========================================
// 1. Definitionen: Lauflicht (D2 bis D5)
// ==========================================
const int ledPins[] = {2, 3, 4, 5};
const int numLedPins = sizeof(ledPins) / sizeof(ledPins[0]);
const unsigned long ledStepInterval = 500; // ms pro Schritt
unsigned long prevLedMillis = 0;
int currentLedStep = 0; // Verwaltet den Zustand im Lauflicht-Ablauf

// ==========================================
// 2. Definitionen: ADC-Logger (A0 bis A7)
// ==========================================
const uint8_t analogPins[] = {A0, A1, A2, A3, A4, A5, A6, A7};
const uint8_t numAnalogPins = sizeof(analogPins) / sizeof(analogPins[0]);
const unsigned long adcInterval = 500; // ms zwischen ADC-Messungen
unsigned long prevAdcMillis = 0;

// ==========================================
// 3. Definitionen: Multiplexer 74HC4051
// ==========================================
const uint8_t pinS0 = 7;
const uint8_t pinS1 = 8;
const uint8_t pinS2 = 9;
const uint8_t pinSignal = 6;
const unsigned long muxInterval = 1500; // ms zwischen Mux-Abfragen
unsigned long prevMuxMillis = 0;

void selectMuxChannel(uint8_t channel) {
  digitalWrite(pinS0, bitRead(channel, 0));
  digitalWrite(pinS1, bitRead(channel, 1));
  digitalWrite(pinS2, bitRead(channel, 2));
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // Warten auf Verbindung (falls erforderlich)
  }

  // Setup Lauflicht
  for (int i = 0; i < numLedPins; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  // Setup Multiplexer
  pinMode(pinS0, OUTPUT);
  pinMode(pinS1, OUTPUT);
  pinMode(pinS2, OUTPUT);
  pinMode(pinSignal, INPUT);

  Serial.println("--- System gestartet ---");
}

void loop() {
  unsigned long currentMillis = millis();

  // ==========================================
  // Task 1: Lauflicht (D2 - D5)
  // ==========================================
  if (currentMillis - prevLedMillis >= ledStepInterval) {
    prevLedMillis = currentMillis;

    // Vorherigen Pin ausschalten
    int previousStep = (currentLedStep == 0) ? (numLedPins - 1) : (currentLedStep - 1);
    digitalWrite(ledPins[previousStep], LOW);

    // Aktuellen Pin einschalten
    digitalWrite(ledPins[currentLedStep], HIGH);
    Serial.print("[Lauflicht] Pin D");
    Serial.print(ledPins[currentLedStep]);
    Serial.println(" -> HIGH");

    // Index für den nächsten Durchlauf erhöhen
    currentLedStep = (currentLedStep + 1) % numLedPins;
  }

  // ==========================================
  // Task 2: ADC-Logger (A0 - A7)
  // ==========================================
  if (currentMillis - prevAdcMillis >= adcInterval) {
    prevAdcMillis = currentMillis;

    Serial.print("[ADC] ");
    for (uint8_t i = 0; i < numAnalogPins; i++) {
      Serial.print("A");
      Serial.print(i);
      Serial.print(":");
      Serial.print(analogRead(analogPins[i]));
      if (i < numAnalogPins - 1) {
        Serial.print(" | ");
      }
    }
    Serial.println();
    delay(1000);
  }

  // ==========================================
  // Task 3: Multiplexer (74HC4051)
  // ==========================================
  if (currentMillis - prevMuxMillis >= muxInterval) {
    prevMuxMillis = currentMillis;

    Serial.print("[MUX] Kanäle: ");
    for (uint8_t ch = 0; ch < 8; ch++) {
      selectMuxChannel(ch);
      delayMicroseconds(5); // Kurze Einschwingzeit

      int status = digitalRead(pinSignal);
      Serial.print("CH");
      Serial.print(ch);
      Serial.print("=");
      Serial.print(status);
      if (ch < 7) {
        Serial.print(" | ");
      }
    }    
    Serial.println();
  }
  delay(1000);
}