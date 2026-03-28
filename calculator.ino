#include <math.h> // Wird für die Modulo-Rechnung (fmod) von Kommazahlen benötigt

void runCalculator() {
  double num1 = 0.0;
  double num2 = 0.0;
  double result = 0.0;
  
  int opIndex = 0; // 0=+, 1=-, 2=*, 3=/, 4=%
  const char ops[] = {'+', '-', '*', '/', '%'};
  
  int state = 0; // 0 = num1 eingeben, 1 = num2 eingeben, 2 = Ergebnis anzeigen
  bool commaMode = false; // True = Wir bearbeiten die Nachkommastellen
  
  long lastEnc = encoderPos / ENC_STEPS;
  unsigned long lastMoveTime = millis();
  
  bool needsRedraw = true;

  // UI Aufbau
  tft.fillScreen(COLOR_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(COLOR_TITLE);
  tft.setCursor(100, 10);
  tft.print("TASCHENRECHNER");
  tft.drawFastHLine(0, 45, 480, COLOR_LINE);
  
  // Hotkey Legende
  tft.setTextSize(2);
  tft.setTextColor(COLOR_INACTIVE);
  tft.setCursor(10, 270);
  tft.print("P1: Komma  P2: Operator  RAD: =");

  delay(200); // Entprellen nach dem Start

  while(true) {
    // --- NOTAUSGANG ---
    if (digitalRead(BTN_MENU) == LOW) {
      delay(200);
      return;
    }

    // --- ENCODER DREHUNG (Mit Beschleunigung!) ---
    long currentEnc = encoderPos / ENC_STEPS;
    long diff = currentEnc - lastEnc;

    if (diff != 0 && state != 2) {
      unsigned long now = millis();
      unsigned long deltaT = now - lastMoveTime;
      lastMoveTime = now;

      // Die dynamische Beschleunigung (Acceleration)
      double stepSize = 1.0;
      if (deltaT < 15) stepSize = 50.0;       // Sehr schnell = 50er Schritte
      else if (deltaT < 40) stepSize = 10.0;  // Mittel = 10er Schritte

      // Wenn Komma-Modus an ist, machen wir die Schritte hundertstel-klein
      if (commaMode) {
        stepSize = stepSize / 100.0;
      }

      // Wert zur aktiven Zahl addieren/subtrahieren
      if (state == 0) {
        num1 += diff * stepSize;
        if (num1 > 99999.99) num1 = 99999.99;
        if (num1 < -99999.99) num1 = -99999.99;
      } else if (state == 1) {
        num2 += diff * stepSize;
        if (num2 > 99999.99) num2 = 99999.99;
        if (num2 < -99999.99) num2 = -99999.99;
      }
      
      lastEnc = currentEnc;
      needsRedraw = true;
    } else if (diff != 0) {
      // Wenn wir im Ergebnis-Screen drehen, tun wir nichts, synchronisieren aber den Encoder
      lastEnc = currentEnc;
    }

    // --- BTN_P1: KOMMA MODUS UMSCHALTEN ---
    if (digitalRead(BTN_P1) == LOW) {
      playBuzzer(1500, 20); // Kurzes Feedback
      commaMode = !commaMode;
      needsRedraw = true;
      delay(250); // Entprellen
    }

    // --- BTN_P2: RECHENART & ZU NUM2 WECHSELN ---
    if (digitalRead(BTN_P2) == LOW) {
      playBuzzer(1000, 20);
      opIndex = (opIndex + 1) % 5;
      if (state == 0) {
        state = 1; // Wir wechseln automatisch zur zweiten Zahl
        commaMode = false; // Komma für die zweite Zahl resetten
      }
      if (state == 2) {
        // Wenn man im Ergebnis ist und Operator drückt, rechnet man mit dem Ergebnis weiter!
        num1 = result;
        num2 = 0.0;
        state = 1; 
      }
      needsRedraw = true;
      delay(250);
    }

    // --- ENCODER RAD DRÜCKEN: BERECHNEN ODER RESET ---
    if (digitalRead(ENC_BTN) == LOW) {
      if (state == 0 || state == 1) {
        // Ausrechnen!
        playBuzzer(2000, 50);
        if (opIndex == 0) result = num1 + num2;
        if (opIndex == 1) result = num1 - num2;
        if (opIndex == 2) result = num1 * num2;
        if (opIndex == 3) {
          if (num2 == 0) result = 0.0; // Div by Zero abfangen
          else result = num1 / num2;
        }
        if (opIndex == 4) {
          if (num2 == 0) result = 0.0;
          else result = fmod(num1, num2); // Modulo für Kommazahlen
        }
        state = 2; // Ergebnis anzeigen
      } else if (state == 2) {
        // Reset für neue Rechnung
        playBuzzer(500, 50);
        num1 = 0.0;
        num2 = 0.0;
        result = 0.0;
        opIndex = 0;
        commaMode = false;
        state = 0;
      }
      needsRedraw = true;
      delay(300);
    }

    // --- ZEICHNEN DER OBERFLÄCHE (Delta-Update) ---
    if (needsRedraw) {
      tft.setTextSize(4);
      
      // Nummer 1
      tft.fillRect(10, 60, 460, 35, COLOR_BLACK);
      tft.setCursor(10, 60);
      if (state == 0) tft.setTextColor(commaMode ? 0xF800 : COLOR_GHOST); // Rot bei Komma, Grün aktiv
      else tft.setTextColor(COLOR_WHITE);
      tft.print(num1, 2); // 2 Nachkommastellen

      // Operator
      tft.fillRect(10, 105, 50, 35, COLOR_BLACK);
      tft.setCursor(10, 105);
      tft.setTextColor(COLOR_LINE);
      tft.print(ops[opIndex]);

      // Nummer 2
      tft.fillRect(10, 150, 460, 35, COLOR_BLACK);
      tft.setCursor(10, 150);
      if (state == 1) tft.setTextColor(commaMode ? 0xF800 : COLOR_GHOST);
      else tft.setTextColor(COLOR_WHITE);
      if (state >= 1) tft.print(num2, 2);

      // Ergebnis-Strich
      tft.drawFastHLine(10, 195, 460, COLOR_WHITE);

      // Ergebnis
      tft.fillRect(10, 210, 460, 35, COLOR_BLACK);
      tft.setCursor(10, 210);
      if (state == 2) {
        tft.setTextColor(COLOR_TITLE);
        tft.print(result, 2);
      }
      
      needsRedraw = false;
    }
  }
}