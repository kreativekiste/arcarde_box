// --- PROTOTYP ---
void playMillisGame();

// --- VARIABLEN ---
enum MillisStatus { MG_START, MG_WAITING, MG_ACTION, MG_RESULT };
MillisStatus mgStatus = MG_START;

unsigned long mgStartTime = 0;
float mgP1Time = 0;
float mgP2Time = 0;

// Layout-Konstanten für den Smiley
#define SM_X 240
#define SM_Y 160
#define SM_R 60 // Radius (ca. 1/5 der Displayhöhe)

// --- HILFSFUNKTION: SMILEY ZEICHNEN ---
void mgDrawSmiley(uint16_t color) {
  // Kopf
  tft.fillCircle(SM_X, SM_Y, SM_R, color);
  // Augen (schwarz)
  tft.fillCircle(SM_X - 20, SM_Y - 15, 6, COLOR_BLACK);
  tft.fillCircle(SM_X + 20, SM_Y - 15, 6, COLOR_BLACK);
  // Mund (einfacher Balken für Geschwindigkeit)
  tft.fillRect(SM_X - 25, SM_Y + 15, 50, 8, COLOR_BLACK);
}

void mgClearSmiley() {
  // Nur den Bereich des Smileys schwarz übermalen
  tft.fillCircle(SM_X, SM_Y, SM_R + 5, COLOR_BLACK);
}

void mgDrawStart() {
  tft.fillScreen(COLOR_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(COLOR_WHITE);
  tft.setCursor(120, 30);
  tft.print("REAKTIONSTEST");
  
  tft.drawFastHLine(0, 70, 480, COLOR_LINE);
  
  tft.setTextSize(2);
  tft.setTextColor(COLOR_GHOST);
  tft.setCursor(110, 240);
  tft.print("BEIDE SPIELER DRUECKEN");
  tft.setCursor(150, 270);
  tft.print("ZUM STARTEN!");
  
  // Kleiner Vorschau-Smiley in Grau
  mgDrawSmiley(COLOR_INACTIVE);
}

void playMillisGame() {
  mgStatus = MG_START;
  mgDrawStart();

  while (true) {
    if (digitalRead(BTN_MENU) == LOW) return;

    switch (mgStatus) {
      case MG_START:
        if (digitalRead(BTN_P1) == LOW && digitalRead(BTN_P2) == LOW) {
          tft.fillScreen(COLOR_BLACK);
          tft.setTextColor(COLOR_WHITE);
          tft.setTextSize(4);
          tft.setCursor(160, 140);
          tft.print("READY?");
          delay(1000);
          tft.fillScreen(COLOR_BLACK);
          
          unsigned long waitTime = random(500, 5000);
          unsigned long waitStart = millis();
          mgStatus = MG_WAITING;
          
          while (millis() - waitStart < waitTime) {
            if (digitalRead(BTN_MENU) == LOW) return;
            // Foul-Erkennung
            if (digitalRead(BTN_P1) == LOW) { mgP1Time = -1; break; }
            if (digitalRead(BTN_P2) == LOW) { mgP2Time = -1; break; }
          }
          
          if (mgP1Time == -1 || mgP2Time == -1) {
            mgStatus = MG_RESULT;
          } else {
            // Signal-Farben (Neon-Look)
            uint16_t colors[] = {0xF81F, 0x07E0, 0x07FF, 0xFFE0}; 
            mgDrawSmiley(colors[random(0, 4)]);
            mgStartTime = millis();
            mgP1Time = 0;
            mgP2Time = 0;
            mgStatus = MG_ACTION;
          }
        }
        break;

      case MG_ACTION:
        if (digitalRead(BTN_P1) == LOW && mgP1Time == 0) {
          mgP1Time = (millis() - mgStartTime) / 1000.0;
        }
        if (digitalRead(BTN_P2) == LOW && mgP2Time == 0) {
          mgP2Time = (millis() - mgStartTime) / 1000.0;
        }
        if (mgP1Time > 0 && mgP2Time > 0) {
          mgStatus = MG_RESULT;
        }
        break;

      case MG_RESULT:
        delay(200); // Kurz warten für das "Ankommen"
        mgClearSmiley();
        tft.setTextSize(4);
        
        if (mgP1Time < 0) { 
           tft.setTextColor(0xF800); tft.setCursor(80, 80); tft.print("P1: FOUL!");
           tft.setTextColor(0x07E0); tft.setCursor(80, 180); tft.print("P2 SIEGT!");
        } 
        else if (mgP2Time < 0) { 
           tft.setTextColor(0x07E0); tft.setCursor(80, 80); tft.print("P1 SIEGT!");
           tft.setTextColor(0xF800); tft.setCursor(80, 180); tft.print("P2: FOUL!");
        }
        else {
          if (mgP1Time < mgP2Time) {
            tft.setTextColor(0x07FF); tft.setCursor(130, 50); tft.print("P1 GEWINNT");
          } else {
            tft.setTextColor(0x07FF); tft.setCursor(130, 50); tft.print("P2 GEWINNT");
          }
          
          tft.setTextSize(3);
          tft.setTextColor(COLOR_WHITE);
          tft.setCursor(100, 130); tft.print("P1: "); tft.print(mgP1Time, 2); tft.print("s");
          tft.setCursor(100, 200); tft.print("P2: "); tft.print(mgP2Time, 2); tft.print("s");
        }

        tft.setTextSize(2);
        tft.setTextColor(COLOR_INACTIVE);
        tft.setCursor(90, 280);
        tft.print("START-KNOPF FUER NEUSTART");
        
        while(digitalRead(BTN_START) == HIGH) {
          if (digitalRead(BTN_MENU) == LOW) return;
        }
        mgP1Time = 0; mgP2Time = 0;
        mgDrawStart();
        mgStatus = MG_START;
        delay(500);
        break;
    }
    delay(1);
  }
}