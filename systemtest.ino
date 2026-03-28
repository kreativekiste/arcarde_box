void runSystemTest() {
  const int testItems = 4;
  const char* testNames[] = {"NEO Pixel", "Buzzer", "Taster", "RTC"};
  
  //Namen der 7 Effekte für das Display
  const char* neoEffectNames[] = {"Color Wipe", "Theater", "Rainbow", "Lauflicht", "Fade", "Breathing", "Sparkle"};
  
  int currentTest = 0;
  long lastTestEnc = -999;
  
  // --- VARIABLEN FÜR STRANDTEST ANIMATIONEN ---
  unsigned long lastNeoTime = 0;
  int neoAnimState = 0; 
  int lastNeoAnimState = -1; 
  long neoStep = 0;
  int neoSubStep = 0;
  int neoCycle = 0;
  int neoDelay = 30; 
  
  int buzzFreq = 200;
  int buzzDir = 10;
  
  int rtcSetMode = 0; 
  long rtcLastEnc = 0;

  tft.fillScreen(COLOR_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(COLOR_TITLE);
  tft.setCursor(120, 10);
  tft.print("HARDWARETEST"); 
  tft.drawFastHLine(0, 45, 480, COLOR_LINE);

  // Initiales Menü zeichnen
  for(int i = 0; i < testItems; i++) {
    tft.setCursor(55, 60 + i * 35);
    tft.setTextColor(COLOR_INACTIVE);
    tft.print(testNames[i]);
  }

  delay(500); 

  bool lastEncBtnState = HIGH;

  while(true) {
    // EXIT
    if (digitalRead(BTN_MENU) == LOW) {
      noTone(PIN_BUZZER);
      pixels.clear();
      pixels.show();
      return; 
    }

    // --- 1. MENÜ-NAVIGATION ---
    if (rtcSetMode == 0) {
      int sel = (encoderPos / ENC_STEPS) % testItems;
      if (sel < 0) sel += testItems;
      
      if (sel != currentTest) {
        
        tft.setTextSize(3);

        // Alte Zeile löschen 
        int oldY = 60 + currentTest * 35;
        tft.fillRect(10, oldY - 5, 230, 40, COLOR_BLACK);
        
        tft.setCursor(55, oldY);
        tft.setTextColor(COLOR_INACTIVE);
        tft.print(testNames[currentTest]);
        
        currentTest = sel;
        
        // Neue Zeile zeichnen
        int newY = 60 + currentTest * 35;
        tft.fillRect(10, newY - 5, 230, 40, COLOR_BLACK);
        
        drawGhost(10, newY, COLOR_GHOST);
        tft.setCursor(55, newY);
        tft.setTextColor(COLOR_GHOST);
        tft.print(testNames[currentTest]);
        
        // Hardware Resets beim Wechsel
        noTone(PIN_BUZZER);
        pixels.clear();
        pixels.show();
        tft.fillRect(250, 60, 230, 200, COLOR_BLACK); // Rechte Bildschirmhälfte putzen
        
        // NeoPixel Animationen sauber zurücksetzen
        neoAnimState = 0; 
        lastNeoAnimState = -1; // Erzwingt das sofortige Zeichnen des Namens
        neoStep = 0; 
        neoCycle = 0; 
        neoSubStep = 0;
        neoDelay = 30;
      }
    }

    // --- 2. NEOPIXEL TEST (7 Animationen) ---
    if (currentTest == 0) {
      
      if (neoAnimState != lastNeoAnimState) {
        tft.fillRect(250, 60, 230, 30, COLOR_BLACK); // Alten Text löschen
        tft.setTextSize(2);
        tft.setTextColor(COLOR_WHITE);
        tft.setCursor(250, 65);
        tft.print(neoEffectNames[neoAnimState]);     // Neuen Namen schreiben
        lastNeoAnimState = neoAnimState;
      }

      if (millis() - lastNeoTime > neoDelay) {
        lastNeoTime = millis();
        
        // Effekt 0: Color Wipe
        if (neoAnimState == 0) {
          neoDelay = 40; 
          uint32_t c;
          if (neoCycle == 0) c = pixels.Color(255, 0, 0);
          else if (neoCycle == 1) c = pixels.Color(0, 255, 0);
          else c = pixels.Color(0, 0, 255);
          
          pixels.setPixelColor(neoStep, c);
          pixels.show();
          neoStep++;
          if (neoStep >= NUM_PIXELS) {
            neoStep = 0; neoCycle++;
            if (neoCycle >= 3) { neoCycle = 0; neoAnimState = 1; pixels.clear(); }
          }
        }
        
        // Effekt 1: Theater Chase
        else if (neoAnimState == 1) {
          neoDelay = 50; 
          neoSubStep++;
          if (neoSubStep > 2) { 
            neoSubStep = 0;
            pixels.clear();
            for(int i = 0; i < NUM_PIXELS; i += 3) {
              if (i + neoStep < NUM_PIXELS) pixels.setPixelColor(i + neoStep, pixels.Color(127, 127, 127)); 
            }
            pixels.show();
            neoStep++;
            if (neoStep >= 3) {
              neoStep = 0; neoCycle++;
              if (neoCycle >= 10) { neoCycle = 0; neoAnimState = 2; neoStep = 0; }
            }
          }
        }
        
        // Effekt 2: Rainbow
        else if (neoAnimState == 2) {
          neoDelay = 20; 
          for(int i = 0; i < NUM_PIXELS; i++) {
            byte pos = 255 - ((neoStep + (i * 256 / NUM_PIXELS)) & 255);
            uint32_t c;
            if(pos < 85) c = pixels.Color(255 - pos * 3, 0, pos * 3);
            else if(pos < 170) { pos -= 85; c = pixels.Color(0, pos * 3, 255 - pos * 3); }
            else { pos -= 170; c = pixels.Color(pos * 3, 255 - pos * 3, 0); }
            pixels.setPixelColor(i, c);
          }
          pixels.show();
          neoStep = (neoStep + 3) & 255; 
          neoCycle++;
          if (neoCycle >= 200) { neoCycle = 0; neoAnimState = 3; neoStep = 0; pixels.clear(); }
        }

        // Effekt 3: Lauflicht (Schnell)
        else if (neoAnimState == 3) {
          neoDelay = 25; 
          uint32_t colors[] = {pixels.Color(255,0,0), pixels.Color(0,255,0), pixels.Color(0,0,255), 
                               pixels.Color(255,255,0), pixels.Color(0,255,255), pixels.Color(255,0,255)};
          
          pixels.clear();
          pixels.setPixelColor(neoStep, colors[neoCycle % 6]); 
          pixels.show();
          
          neoStep++;
          if (neoStep >= NUM_PIXELS) {
            neoStep = 0;
            neoCycle++;
            if (neoCycle >= 12) { neoCycle = 0; neoAnimState = 4; neoStep = 0; } 
          }
        }

        // Effekt 4: Fade (Langsam)
        else if (neoAnimState == 4) {
          neoDelay = 40; 
          uint32_t color = pixels.gamma32(pixels.ColorHSV((uint16_t)neoStep));
          
          for(int i = 0; i < NUM_PIXELS; i++) pixels.setPixelColor(i, color);
          pixels.show();
          
          neoStep += 256; 
          if (neoStep >= 65536) { 
            neoStep = 0; 
            neoCycle++;
            if (neoCycle >= 2) { neoCycle = 0; neoAnimState = 5; neoStep = 0; neoSubStep = 1; pixels.clear(); }
          }
        }

        // Effekt 5: Breathing (Langsam)
        else if (neoAnimState == 5) {
          neoDelay = 30; 
          for(int i = 0; i < NUM_PIXELS; i++) pixels.setPixelColor(i, pixels.Color(0, neoStep, neoStep));
          pixels.show();
          
          neoStep += neoSubStep * 4; 
          
          if (neoStep >= 255) {
            neoStep = 255;
            neoSubStep = -1; 
          } else if (neoStep <= 0) {
            neoStep = 0;
            neoSubStep = 1; 
            neoCycle++;
            if (neoCycle >= 3) { neoCycle = 0; neoAnimState = 6; neoStep = 0; pixels.clear(); } 
          }
        }

        // Effekt 6: Sparkle (Mittel)
        else if (neoAnimState == 6) {
          neoDelay = 60; 
          pixels.clear();
          
          pixels.setPixelColor(random(NUM_PIXELS), pixels.Color(random(255), random(255), random(255)));
          pixels.setPixelColor(random(NUM_PIXELS), pixels.Color(random(255), random(255), random(255)));
          pixels.show();
          
          neoCycle++;
          if (neoCycle >= 60) { neoCycle = 0; neoAnimState = 0; neoStep = 0; pixels.clear(); } 
        }
      }
    } 
    
    // --- 3. BUZZER TEST ---
    else if (currentTest == 1) {
      tone(PIN_BUZZER, buzzFreq);
      buzzFreq += buzzDir;
      if(buzzFreq > 2000 || buzzFreq < 200) buzzDir = -buzzDir;
      delay(3); 
    } 
    
    // --- 4. TASTER TEST ---
    else if (currentTest == 2) {
      bool anyPressed = (digitalRead(BTN_P1) == LOW || digitalRead(BTN_P2) == LOW || 
                         digitalRead(BTN_START) == LOW || digitalRead(BTN_MENU) == LOW || 
                         digitalRead(ENC_BTN) == LOW);
                         
      if(anyPressed) {
        tft.fillCircle(420, 160, 20, 0xF800); 
      } else {
        tft.fillCircle(420, 160, 20, COLOR_BLACK);
      }
    } 
    
    // --- 5. RTC TEST ---
    else if (currentTest == 3) {
      RTCTime t;
      RTC.getTime(t);
      
      tft.setTextSize(3);
      tft.setCursor(250, 100);
      
      if (rtcSetMode == 1) tft.setTextColor(0xF800, COLOR_BLACK); 
      else tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
      if (t.getHour() < 10) tft.print("0");
      tft.print(t.getHour());
      tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
      tft.print(":");
      
      if (rtcSetMode == 2) tft.setTextColor(0xF800, COLOR_BLACK); 
      else tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
      if (t.getMinutes() < 10) tft.print("0");
      tft.print(t.getMinutes());
      tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
      tft.print(":");
      
      if (t.getSeconds() < 10) tft.print("0");
      tft.print(t.getSeconds());
      
      tft.setTextSize(2);
      tft.setCursor(250, 140);
      tft.setTextColor(COLOR_INACTIVE, COLOR_BLACK);
      if(t.getDayOfMonth() < 10) tft.print("0");
      tft.print(t.getDayOfMonth()); tft.print("."); 
      
      int currentMonth = (int)t.getMonth();
      if(currentMonth < 10) tft.print("0");
      tft.print(currentMonth); tft.print(".");
      
      tft.print(t.getYear());

      bool currentEncBtn = digitalRead(ENC_BTN);
      if (currentEncBtn == LOW && lastEncBtnState == HIGH) {
        rtcSetMode++;
        if (rtcSetMode > 2) rtcSetMode = 0; 
        rtcLastEnc = encoderPos / ENC_STEPS;
        delay(50);
      }
      lastEncBtnState = currentEncBtn;

      if (rtcSetMode > 0) {
        long currentEnc = encoderPos / ENC_STEPS;
        long diff = currentEnc - rtcLastEnc;
        
        if (diff != 0) {
          int newH = t.getHour();
          int newM = t.getMinutes();
          
          if (rtcSetMode == 1) newH = (newH + diff + 24) % 24;
          if (rtcSetMode == 2) newM = (newM + diff + 60) % 60;
          
          RTCTime newTime(t.getDayOfMonth(), t.getMonth(), t.getYear(), newH, newM, 0, t.getDayOfWeek(), SaveLight::SAVING_TIME_INACTIVE);
          RTC.setTime(newTime);
          rtcLastEnc = currentEnc;
        }
      }
    }
  }
}