// --- PROTOTYP ---
void playZahlenRaten();

// --- VARIABLEN ---
int zrTarget = 0;
int zrTries = 0;
int zrCurrentGuess = 50;
int zrMin = 1;
int zrMax = 100;
bool zrWon = false;
bool zrActive = false;
int zrLastDisplayedGuess = -1;

// --- GRAFIK-HILFSFUNKTIONEN ---

void zrDrawUI() {
  tft.fillScreen(COLOR_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(COLOR_TITLE);
  tft.setCursor(110, 20);
  tft.print("ZAHLEN RATEN");
  tft.drawFastHLine(0, 55, 480, COLOR_LINE);
  
  tft.setTextSize(2);
  tft.setTextColor(COLOR_INACTIVE);
  tft.setCursor(20, 280);
  tft.print("BEREICH: 1 - 100");
}

void zrDrawHint(String msg, uint16_t color, bool up) {
  // Alten Bereich löschen
  tft.fillRect(30, 80, 150, 180, COLOR_BLACK);
  
  tft.setTextColor(color);
  tft.setTextSize(3);
  tft.setCursor(30, 80);
  tft.print(msg);

  // Pfeil zeichnen
  if (up) {
    // Pfeil nach oben
    tft.fillTriangle(100, 130, 70, 170, 130, 170, color);
    tft.fillRect(90, 170, 20, 40, color);
  } else {
    // Pfeil nach unten
    tft.fillRect(90, 130, 20, 40, color);
    tft.fillTriangle(100, 210, 70, 170, 130, 170, color);
  }
}

void zrShowWin() {
  tft.fillScreen(0x07E0); // Grün
  tft.setTextColor(COLOR_BLACK);
  tft.setTextSize(5);
  tft.setCursor(80, 100);
  tft.print("TREFFER!");
  
  tft.setTextSize(3);
  tft.setCursor(80, 180);
  tft.print("VERSUCHE: ");
  tft.print(zrTries);
  
  delay(4000);
}

void zrReset() {
  zrTarget = random(1, 101);
  zrTries = 0;
  zrMin = 1;
  zrMax = 100;
  zrCurrentGuess = 50;
  zrWon = false;
  zrActive = true;
  zrLastDisplayedGuess = -1;
  zrDrawUI();
  
  tft.setTextSize(2);
  tft.setTextColor(COLOR_WHITE);
  tft.setCursor(220, 80);
  tft.print("DEIN TIPP:");
}

// --- HAUPTFUNKTION ---
void playZahlenRaten() {
  zrReset();
  
  while(true) {
    if (digitalRead(BTN_MENU) == LOW) return;

    // 1. ZAHL AUSWÄHLEN (Encoder)
    long newPos = encoderPos / ENC_STEPS;
    // Wir begrenzen die Auswahl auf 1-100
    zrCurrentGuess = abs(newPos % 100) + 1;

    // Nur bei Änderung zeichnen (Anti-Flackern)
    if (zrCurrentGuess != zrLastDisplayedGuess) {
      tft.fillRect(220, 110, 180, 100, COLOR_BLACK); // Zahl-Bereich löschen
      tft.setTextSize(9);
      tft.setTextColor(COLOR_WHITE);
      tft.setCursor(220, 120);
      if(zrCurrentGuess < 10) tft.print("0"); // Führende Null für Optik
      tft.print(zrCurrentGuess);
      zrLastDisplayedGuess = zrCurrentGuess;
    }

    // 2. TIPP ABGEBEN (Encoder-Knopf)
    if (digitalRead(ENC_BTN) == LOW) {
      delay(250); // Entprellen
      zrTries++;
      
      if (zrCurrentGuess == zrTarget) {
        zrShowWin();
        zrReset(); // Neue Runde
      } 
      else if (zrCurrentGuess > zrTarget) {
        // Zu HOCH -> Pfeil nach UNTEN
        zrDrawHint("ZU HOCH", 0xF800, false); // Rot
      } 
      else {
        // Zu TIEF -> Pfeil nach OBEN
        zrDrawHint("ZU TIEF", 0x07FF, true);  // Cyan/Blau
      }
    }
    delay(10);
  }
}