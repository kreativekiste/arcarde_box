#include <EEPROM.h>

// --- VARIABLEN FÜR LED & SOUND STEUERUNG ---
int ledMode = 0; 
int ledColorIdx = 1; 
int ledBrightness = 30; 
unsigned long lastLedUpdate = 0;
uint16_t ledAnimStep = 0;

const int NUM_COLORS = 20;
const char* colorNames[NUM_COLORS] = {
  "Rot", "Orange", "Gelb", "Limette", "Gruen", 
  "Mint", "Cyan", "Hellblau", "Blau", "Lila", 
  "Magenta", "Pink", "Weiss", "Warmweiss", "Kaltweiss", 
  "Gold", "Tuerkis", "Cyber-Pink", "Neon-Gruen", "Blutrot"
};

const uint8_t colorRGB[NUM_COLORS][3] = {
  {255, 0, 0}, {255, 100, 0}, {255, 255, 0}, {128, 255, 0}, {0, 255, 0}, 
  {0, 255, 128}, {0, 255, 255}, {0, 128, 255}, {0, 0, 255}, {128, 0, 255}, 
  {255, 0, 255}, {255, 0, 128}, {255, 255, 255}, {255, 200, 150}, {200, 220, 255}, 
  {255, 215, 0}, {64, 224, 208}, {255, 20, 147}, {57, 255, 20}, {138, 3, 3}      
};

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  if(WheelPos < 170) { WheelPos -= 85; return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3); }
  WheelPos -= 170; return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void applySolidColor() {
  pixels.clear();
  uint32_t c = pixels.Color(colorRGB[ledColorIdx][0], colorRGB[ledColorIdx][1], colorRGB[ledColorIdx][2]);
  for(int i=0; i<NUM_PIXELS; i++) pixels.setPixelColor(i, c);
  pixels.show();
}

void setMenuLeds() {
  if (ledMode == 3) { pixels.clear(); pixels.show(); } 
  else { applySolidColor(); }
}

// LÄDT DATEN AUS DEM EEPROM
void setupLeds() {
  // EEPROM Speicherorte: 0=Modus, 1=Farbe, 2=Helligkeit, 3=Sound
  ledMode = EEPROM.read(0);
  
  // Wenn das EEPROM leer/neu ist (Wert 255), setzen wir Standardwerte!
  if (ledMode > 3) {
    ledMode = 0;
    ledColorIdx = 1; // Orange
    ledBrightness = 30;
    soundEnabled = true;
    
    // Erste Speicherung
    EEPROM.write(0, ledMode);
    EEPROM.write(1, ledColorIdx);
    EEPROM.write(2, ledBrightness);
    EEPROM.write(3, 1); // 1 = Sound an
  } else {
    // Wenn Daten existieren, laden wir sie
    ledColorIdx = EEPROM.read(1);
    ledBrightness = EEPROM.read(2);
    soundEnabled = (EEPROM.read(3) == 1);
  }

  pixels.setBrightness(ledBrightness);
  setMenuLeds();
}

// SPEICHERT DATEN INS EEPROM
void saveSettings() {
  EEPROM.write(0, ledMode);
  EEPROM.write(1, ledColorIdx);
  EEPROM.write(2, ledBrightness);
  EEPROM.write(3, soundEnabled ? 1 : 0);
}

void handleLeds() {
  if (ledMode == 1) { 
    if (millis() - lastLedUpdate > 20) {
      lastLedUpdate = millis();
      for(int i=0; i<NUM_PIXELS; i++) pixels.setPixelColor(i, Wheel(((i * 256 / NUM_PIXELS) + ledAnimStep) & 255));
      pixels.show();
      ledAnimStep = (ledAnimStep + 2) & 255;
    }
  } 
  else if (ledMode == 2) { 
    if (millis() - lastLedUpdate > 60) {
      lastLedUpdate = millis();
      pixels.clear();
      uint32_t c = pixels.Color(colorRGB[ledColorIdx][0], colorRGB[ledColorIdx][1], colorRGB[ledColorIdx][2]);
      for(int i=0; i<NUM_PIXELS; i+=3) {
        int pos = i + (ledAnimStep % 3);
        if(pos < NUM_PIXELS) pixels.setPixelColor(pos, c);
      }
      pixels.show();
      ledAnimStep++;
    }
  }
}

// --- DAS VERSTECKTE SETUP MENÜ ---
void runSetupMenu() {
  const int menuItems = 5;
  const char* menuNames[] = {"FESTE FARBE", "EFFEKT", "HELLIGKEIT", "TON", "ZURUECK"};
  int currentItem = 0;
  long lastEnc = -999;
  
  int editMode = 0; 
  long editStartEnc = 0;

  tft.fillScreen(COLOR_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(COLOR_TITLE);
  tft.setCursor(120, 10);
  tft.print("SYSTEM SETUP");
  tft.drawFastHLine(0, 45, 480, COLOR_LINE);

  for(int i = 0; i < menuItems; i++) {
    tft.setCursor(55, 60 + i * 35);
    tft.setTextColor(COLOR_INACTIVE);
    tft.print(menuNames[i]);
  }

  delay(500); 
  bool lastBtnState = HIGH;

  while(true) {
    handleLeds(); 

    bool btnState = digitalRead(ENC_BTN);
    if (btnState == LOW && lastBtnState == HIGH) {
      delay(50); 
      if (editMode == 0) {
        if (currentItem == 4) { // ZURUECK
          saveSettings();
          setMenuLeds(); 
          return;
        } else {
          editMode = 1; 
          editStartEnc = encoderPos / ENC_STEPS;
        }
      } else {
        editMode = 0; 
        lastEnc = -999; 
        saveSettings(); // Speichern bei jeder Änderung
      }
    }
    lastBtnState = btnState;

    if (digitalRead(BTN_MENU) == LOW) {
      saveSettings();
      setMenuLeds(); 
      return;
    }

    if (editMode == 0) {
      int sel = (encoderPos / ENC_STEPS) % menuItems;
      if (sel < 0) sel += menuItems;
      
      if (sel != currentItem || lastEnc == -999) {
        tft.fillRect(10, 55 + currentItem * 35, 230, 35, COLOR_BLACK);
        tft.setCursor(55, 60 + currentItem * 35);
        tft.setTextColor(COLOR_INACTIVE);
        tft.print(menuNames[currentItem]);
        
        currentItem = sel;
        
        tft.fillRect(10, 55 + currentItem * 35, 230, 35, COLOR_BLACK);
        drawGhost(10, 60 + currentItem * 35, COLOR_GHOST);
        tft.setCursor(55, 60 + currentItem * 35);
        tft.setTextColor(COLOR_GHOST);
        tft.print(menuNames[currentItem]);
        
        lastEnc = sel;
        
        tft.fillRect(250, 60, 230, 200, COLOR_BLACK); 
        tft.setTextSize(3);
        tft.setCursor(260, 60 + currentItem * 35);
        tft.setTextColor(COLOR_WHITE);
        
        if (currentItem == 0) tft.print(colorNames[ledColorIdx]);
        if (currentItem == 1) {
          if (ledMode == 0) tft.print("Statisch");
          else if (ledMode == 1) tft.print("Rainbow");
          else if (ledMode == 2) tft.print("Theater");
          else tft.print("Aus");
        }
        if (currentItem == 2) { tft.print(ledBrightness); tft.print("/255"); }
        if (currentItem == 3) { tft.print(soundEnabled ? "AN" : "AUS"); }
      }
    } else {
      long diff = (encoderPos / ENC_STEPS) - editStartEnc;
      
      if (diff != 0) {
        if (currentItem == 0) { 
          ledColorIdx = (ledColorIdx + diff) % NUM_COLORS;
          if (ledColorIdx < 0) ledColorIdx += NUM_COLORS;
          ledMode = 0; 
          applySolidColor();
        } 
        else if (currentItem == 1) { 
          ledMode = (ledMode + diff) % 4; // Jetzt 4 Modi (inkl Aus)
          if (ledMode < 0) ledMode += 4;
          if (ledMode == 0) applySolidColor();
          if (ledMode == 3) { pixels.clear(); pixels.show(); }
        }
        else if (currentItem == 2) { 
          ledBrightness = constrain(ledBrightness + (diff * 5), 5, 255); 
          pixels.setBrightness(ledBrightness);
          if (ledMode == 0) applySolidColor();
        }
        else if (currentItem == 3) {
          soundEnabled = !soundEnabled; // Toggle AN/AUS
        }

        editStartEnc = encoderPos / ENC_STEPS;
        
        tft.fillRect(250, 60 + currentItem * 35, 230, 35, COLOR_BLACK);
        tft.setCursor(260, 60 + currentItem * 35);
        tft.setTextColor(0xF800); 
        
        if (currentItem == 0) tft.print(colorNames[ledColorIdx]);
        if (currentItem == 1) {
          if (ledMode == 0) tft.print("Statisch");
          else if (ledMode == 1) tft.print("Rainbow");
          else if (ledMode == 2) tft.print("Theater");
          else if (ledMode == 3) tft.print("Aus");
        }
        if (currentItem == 2) { tft.print(ledBrightness); tft.print("/255"); }
        if (currentItem == 3) { tft.print(soundEnabled ? "AN" : "AUS"); }
      }
    }
  }
}