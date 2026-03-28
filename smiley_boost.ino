// --- SMILEY BOOST PAGE ---

// Hilfsfunktion zum Zeichnen des Mundes
void drawSmile(int x, int y, uint16_t color) {
  // Eine dicke, geschwungene Linie als Mund
  // y ist der tiefste Punkt des Mundes
  for (int i = 0; i < 3; i++) {
    tft.drawLine(x - 50, y - 20 + i, x - 30, y + i, color);
    tft.drawLine(x - 30, y + i, x + 30, y + i, color);
    tft.drawLine(x + 30, y + i, x + 50, y - 20 + i, color);
  }
}

void runSmileyBoost() {
  // Farbe Gelb (RGB565) definieren
  const uint16_t COLOR_YELLOW = 0xFFE0;

  tft.fillScreen(COLOR_BLACK);
  drawGhost(440, 10, COLOR_GHOST); // Kleiner Geist zur Deko in die Ecke

  // --- ANFANGSZUSTAND DES SMILEYS ---
  // Großer gelber Kopf
  tft.fillCircle(240, 130, 90, COLOR_YELLOW);
  
  // Zwei schwarze Augen (offen)
  tft.fillCircle(200, 100, 15, COLOR_BLACK); // Links
  tft.fillCircle(280, 100, 15, COLOR_BLACK); // Rechts

  // Der Mund
  drawSmile(240, 180, COLOR_BLACK);

  // Ein kurzer, heller "Ping" Sound
  playBuzzer(1800, 80); 

  // --- ANIMATIONS-PAUSE ---
  delay(1000); // Warte ca. 1 Sekunde

  // --- ZWINKER-ANIMATION (RECHTES AUGE) ---
  // Wir radieren das rechte offene Auge aus (mit Gelb übermalen)
  tft.fillCircle(280, 100, 16, COLOR_YELLOW); 

  // Wir zeichnen ein zwinkerndes Auge (dicke geschwungene Linie)
  for (int i = 0; i < 3; i++) {
    tft.drawLine(260, 100 + i, 280, 110 + i, COLOR_BLACK);
    tft.drawLine(280, 110 + i, 300, 100 + i, COLOR_BLACK);
  }
  
  // Ein tieferer Winking-Sound
  playBuzzer(2000, 50);

  // --- TEXT ANZEIGE ---
  tft.setTextSize(3);
  tft.setTextColor(COLOR_WHITE);
  
  // Der Text (mittig gesetzt)
  // "Du bist gewollt, geliebt" ist 24 chars, centered at x=24
  tft.setCursor(24, 230);
  tft.print("Du bist gewollt, geliebt");

  // "und unperfekt." ist 15 chars, centered at x=105
  tft.setCursor(105, 265);
  tft.print("und unperfekt.");

  delay(300); // Entprellen

  // Warten auf Knopfdruck um zurückzugehen
  while(true) {
    if (digitalRead(ENC_BTN) == LOW || digitalRead(BTN_MENU) == LOW) {
      delay(200);
      return;
    }
    delay(50);
  }
}