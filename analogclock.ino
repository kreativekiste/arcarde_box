void showAnalogClock() {
  tft.fillScreen(COLOR_BLACK);
  isClockShowing = true;
  
  // Zentrum und Radius der Uhr (nach rechts verschoben)
  int centerX = 330; 
  int centerY = 160; 
  int radius = 120;
  
  // Statisches Ziffernblatt zeichnen
  tft.drawCircle(centerX, centerY, radius, COLOR_WHITE);
  for(int i=0; i<12; i++) {
    float angle = i * 30 * PI / 180;
    int x1 = centerX + cos(angle)*105; int y1 = centerY + sin(angle)*105;
    int x2 = centerX + cos(angle)*120; int y2 = centerY + sin(angle)*120;
    tft.drawLine(x1, y1, x2, y2, COLOR_WHITE);
  }

  // Geist in das linke Viereck zeichnen
  drawGhost(70, 140, COLOR_GHOST);

  // Hilfsfunktion für dicke Zeiger
  auto drawThickHand = [&](float deg, int len, int width, uint16_t color) {
    float rad = (deg - 90) * PI / 180; float radPerp = deg * PI / 180; 
    int tipX = centerX + cos(rad) * len; int tipY = centerY + sin(rad) * len;
    int b1X = centerX + cos(radPerp) * width; int b1Y = centerY + sin(radPerp) * width;
    int b2X = centerX - cos(radPerp) * width; int b2Y = centerY - sin(radPerp) * width;
    tft.fillTriangle(b1X, b1Y, b2X, b2Y, tipX, tipY, color);
  };

  float oldSDeg = -1, oldMDeg = -1, oldHDeg = -1;
  int oldDay = -1;
  unsigned long lastSensorUpdate = 0;

  delay(200); // Entprellen beim Start

  while (isClockShowing) {
    handleLeds(); // LED Animationen weiterlaufen lassen
    RTCTime t;
    RTC.getTime(t);
    
    // --- TEMPERATUR & FEUCHTIGKEIT (Alle 5 Sekunden) ---
    if (millis() - lastSensorUpdate > 5000 || lastSensorUpdate == 0) {
      float temp = dht.readTemperature();
      float hum  = dht.readHumidity();
      
      tft.setTextSize(3);
      tft.setTextColor(COLOR_TITLE, COLOR_BLACK);
      
      tft.setCursor(20, 30);
      if(!isnan(temp)) { tft.print(temp, 1); tft.print(" C"); }
      
      tft.setCursor(20, 70);
      if(!isnan(hum)) { tft.print(hum, 0); tft.print(" %"); }
      
      lastSensorUpdate = millis();
    }

    // --- DATUM UNTEN LINKS ---
    if (t.getDayOfMonth() != oldDay) {
      tft.setTextSize(3);
      tft.setTextColor(COLOR_INACTIVE, COLOR_BLACK);
      char dateStr[15];
      sprintf(dateStr, "%02d.%02d.%04d", t.getDayOfMonth(), (int)t.getMonth(), t.getYear());
      tft.setCursor(20, 265);
      tft.print(dateStr);
      oldDay = t.getDayOfMonth();
    }

    // --- UHRZEIGER ---
    float sDeg = t.getSeconds() * 6;
    float mDeg = t.getMinutes() * 6;
    float hDeg = (t.getHour() % 12) * 30 + (t.getMinutes() * 0.5);

    if (sDeg != oldSDeg) {
      // Alte Zeiger löschen (Schwarz übermalen)
      if (oldHDeg != -1) drawThickHand(oldHDeg, 65, 5, COLOR_BLACK);
      if (oldMDeg != -1) drawThickHand(oldMDeg, 100, 3, COLOR_BLACK);
      if (oldSDeg != -1) drawThickHand(oldSDeg, 115, 1, COLOR_BLACK);

      // Neue Zeiger zeichnen
      drawThickHand(hDeg, 65, 5, COLOR_TITLE);
      drawThickHand(mDeg, 100, 3, COLOR_WHITE);
      drawThickHand(sDeg, 115, 1, 0xF800); 
      
      // Mittelpunkt-Pin
      tft.fillCircle(centerX, centerY, 6, COLOR_TITLE);
      tft.fillCircle(centerX, centerY, 3, 0xF800);
      oldSDeg = sDeg; oldMDeg = mDeg; oldHDeg = hDeg;
    }
    
    delay(20); 
    
    // --- TASTENABFRAGE ---
    if (digitalRead(BTN_START) == LOW) {
      isClockShowing = false;
      delay(200);
      runStopwatch(); 
      lastActivityTime = millis();
      tft.fillScreen(COLOR_BLACK);
      lastMenuIndex = -1; 
      setMenuLeds(); 
    } else if (digitalRead(ENC_BTN) == LOW || digitalRead(BTN_MENU) == LOW || 
               digitalRead(BTN_P1) == LOW || digitalRead(BTN_P2) == LOW || 
               (millis() - lastActivityTime < 500)) {
      isClockShowing = false;
      lastActivityTime = millis();
      tft.fillScreen(COLOR_BLACK);
      lastMenuIndex = -1; 
      setMenuLeds(); 
    }
  }
}