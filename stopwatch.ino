void runStopwatch() {
  tft.fillScreen(COLOR_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(COLOR_TITLE);
  tft.setCursor(160, 10);
  tft.print("STOPPUHR");
  tft.drawFastHLine(0, 45, 480, COLOR_LINE);

  // Hotkey Legende
  tft.setTextSize(2);
  tft.setTextColor(COLOR_INACTIVE);
  tft.setCursor(10, 270);
  tft.print("T1: Start/Pause  T2: Runde  T3: Reset");

  bool running = false;
  unsigned long startTime = 0;
  unsigned long elapsedTime = 0;
  
  unsigned long laps[3] = {0, 0, 0};
  
  unsigned long lastDrawTime = 0;
  bool needsFullRedraw = true;
  
  // NEU: Speichert den exakten alten Text, um nur Änderungen zu zeichnen!
  char lastTimeStr[10] = "--------"; 

  delay(300); // Entprellen

  while(true) {
    // --- NOTAUSGANG ---
    if (digitalRead(BTN_MENU) == LOW) {
      playBuzzer(800, 50);
      delay(200);
      return;
    }

    // --- T1: START / PAUSE ---
    if (digitalRead(BTN_START) == LOW) { 
      playBuzzer(1500, 50);
      delay(200); 
      if (running) {
        running = false; 
        needsFullRedraw = true; // Farbe ändert sich auf Weiß
      } else {
        running = true;  
        startTime = millis() - elapsedTime;
        needsFullRedraw = true; // Farbe ändert sich auf Grün
      }
    }

    // --- T2: ZWISCHENZEIT ---
    if (digitalRead(BTN_P1) == LOW) { 
      if (running) {
        playBuzzer(2000, 50);
        delay(200); 
        laps[2] = laps[1];
        laps[1] = laps[0];
        laps[0] = elapsedTime;
        needsFullRedraw = true;
      }
    }

    // --- T3 / RAD: RESET ---
    if (digitalRead(BTN_P2) == LOW || digitalRead(ENC_BTN) == LOW) { 
      playBuzzer(500, 100);
      delay(200); 
      running = false;
      elapsedTime = 0;
      laps[0] = 0; laps[1] = 0; laps[2] = 0; 
      needsFullRedraw = true;
    }

    if (running) {
      elapsedTime = millis() - startTime;
    }

    // --- UI ZEICHNEN ---
    // Update Frequenz erhöht (alle 20ms statt 50ms) für flüssigere Hundertstel!
    if (millis() - lastDrawTime > 20 || needsFullRedraw) {
      lastDrawTime = millis();
      
      unsigned long totalMs = elapsedTime;
      unsigned long ms = (totalMs % 1000) / 10; 
      unsigned long sec = (totalMs / 1000) % 60;
      unsigned long min = (totalMs / 60000);

      char timeStr[10];
      sprintf(timeStr, "%02lu:%02lu.%02lu", min, sec, ms);

      // --- DER ANTI-DISCO-BALL CODE ---
      tft.setTextSize(7);
      
      // Zeichen für Zeichen einzeln prüfen
      for (int i = 0; i < 8; i++) {
        // Nur wenn sich die Ziffer WIRKLICH geändert hat oder ein Farbwechsel ansteht
        if (timeStr[i] != lastTimeStr[i] || needsFullRedraw) {
          
          // X-Position für dieses Zeichen exakt berechnen 
          // (Bei TextSize 7 ist ein Standard-Zeichen genau 42 Pixel breit)
          int charX = 70 + (i * 42); 
          
          tft.setCursor(charX, 75);
          
          // Der Trick: Hintergrundfarbe gleich mitgeben! 
          // So überschreibt das Display alte Pixel, ohne dass es flackert.
          if (running) {
            tft.setTextColor(COLOR_GHOST, COLOR_BLACK); 
          } else {
            tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
          }
          
          tft.print(timeStr[i]);
          lastTimeStr[i] = timeStr[i]; // Änderung merken
        }
      }

      // Zwischenzeiten zeichnen (Hier reicht es ganz normal, da sie nur auf Knopfdruck aktualisiert werden)
      if (needsFullRedraw) {
        tft.setTextSize(3);
        for (int i = 0; i < 3; i++) {
          int yPos = 160 + (i * 35);
          tft.fillRect(100, yPos, 280, 30, COLOR_BLACK); 
          
          if (laps[i] > 0) {
            unsigned long lTotalMs = laps[i];
            unsigned long lMs = (lTotalMs % 1000) / 10;
            unsigned long lSec = (lTotalMs / 1000) % 60;
            unsigned long lMin = (lTotalMs / 60000);
            
            char lapStr[20];
            sprintf(lapStr, "Runde: %02lu:%02lu.%02lu", lMin, lSec, lMs);
            
            // Neueste Runde bekommt eine eigene Farbe
            if (i == 0) tft.setTextColor(COLOR_TITLE, COLOR_BLACK); 
            else tft.setTextColor(COLOR_INACTIVE, COLOR_BLACK);     
            
            tft.setCursor(100, yPos);
            tft.print(lapStr);
          }
        }
        needsFullRedraw = false;
      }
    }
  }
}