// ############################################################
// # PROJECT: ARCADE AUTOMAT V1.73                            #
// # AUTHOR:  [Ronnie Berzins / Kreativekiste.de]             #
// # TECH-SUPPORT: Erstellt in Zusammenarbeit mit Gemini AI   #
// # DATE:    29.03.2026                                      #
// ############################################################


#include <SPI.h>
#include <ILI9486_SPI.h>
#include <Adafruit_NeoPixel.h>
#include "RTC.h"
#include <EEPROM.h> 
#include <DHT.h> 

// --- 1. PROTOTYPEN ---
void playVierGewinnt();
void playMasterDesaster();
void playSlotMaschine();
void playTheDinoGame();
void playMillisGame();
void playZahlenRaten();
void playKlickFit();
void playWuerfel();
void playSimonSagt();
void playBreakout(); 

void runSystemTest(); 
void showAnalogClock(); 
void runSetupMenu(); 
void runCalculator(); 
void runStopwatch();   
void runSmileyBoost(); 
void handleLeds();   
void setupLeds();    
void setMenuLeds(); 

// --- 2. HARDWARE DEFINITIONEN ---
const int TFT_CS  = 10;
const int TFT_DC  = 8;
const int TFT_RST = 9;

const int ENC_A   = 2;
const int ENC_B   = 3;
const int ENC_BTN = 4;

const int BTN_START = A1; 
const int BTN_P1    = A2; 
const int BTN_P2    = A3; 
const int BTN_MENU  = A4; 

const int SEED_PIN  = A0;
const int PIN_NEOPIXEL = 5; 
const int NUM_PIXELS   = 16; 
const int PIN_BUZZER   = 6;

const int DHT_PIN  = 7;    
const int DHT_TYPE = DHT22;

const int ENC_STEPS = 2; 

// --- 3. FARBEN (RGB565) ---
const uint16_t COLOR_BLACK    = 0x0000;
const uint16_t COLOR_WHITE    = 0xFFFF;
const uint16_t COLOR_GHOST    = 0x07E0; 
const uint16_t COLOR_TITLE    = 0x07FF; 
const uint16_t COLOR_LINE     = 0xF81F; 
const uint16_t COLOR_INACTIVE = 0x7BEF; 

ILI9486_SPI tft(TFT_CS, TFT_DC, TFT_RST);
Adafruit_NeoPixel pixels(NUM_PIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
DHT dht(DHT_PIN, DHT_TYPE); 

// --- 4. GLOBALE VARIABLEN ---
volatile long encoderPos = 0;
volatile int lastStateA;
int lastMenuIndex = -1;

const int totalGames = 10; 
const char* gameNames[10] = {
  "VIER GEWINNT", "MASTER DESASTER", "SLOT MASCHINE", 
  "THE DINO GAME", "MILLIS GAME", "ZAHLEN RATEN", 
  "KLICK FIT", "WUERFEL", "SIMON SAGT", "BREAKOUT" 
};

unsigned long lastActivityTime = 0;   
const unsigned long idleTimeout = 7000; 
bool isClockShowing = false;
bool soundEnabled = true; 

// --- 5. INTERRUPT FÜR DEN ENCODER ---
void readEncoder() {
  int currentStateA = digitalRead(ENC_A);
  if (currentStateA != lastStateA) {
    if (digitalRead(ENC_B) != currentStateA) encoderPos++;
    else encoderPos--;
    lastActivityTime = millis(); 
  }
  lastStateA = currentStateA;
}

// --- 6. HILFSFUNKTIONEN ---
void playBuzzer(int freq, int duration) {
  if (soundEnabled) tone(PIN_BUZZER, freq, duration);
}

void syncRTCtoCompileTime() {
  const char time_str[] = __TIME__; 
  const char date_str[] = __DATE__; 
  int h = (time_str[0] - '0') * 10 + (time_str[1] - '0');
  int m = (time_str[3] - '0') * 10 + (time_str[4] - '0');
  int s = (time_str[6] - '0') * 10 + (time_str[7] - '0');
  char monthStr[4]; int d, y;
  sscanf(date_str, "%s %d %d", monthStr, &d, &y);
  int mth = 1;
  const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  for (int i = 0; i < 12; i++) { if (strncmp(monthStr, months[i], 3) == 0) { mth = i + 1; break; } }
  RTCTime compileTime(d, Month(mth), y, h, m, s, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_INACTIVE);
  RTC.setTime(compileTime);
}

void drawGhost(int x, int y, uint16_t color) {
  tft.fillRect(x + 4, y, 10, 14, color);
  tft.fillCircle(x + 9, y + 4, 8, color);
  tft.fillRect(x, y + 8, 18, 8, color);
  tft.fillCircle(x + 4, y + 16, 2, color);
  tft.fillCircle(x + 9, y + 16, 2, color);
  tft.fillCircle(x + 14, y + 16, 2, color);
  tft.fillCircle(x + 6, y + 4, 2, COLOR_WHITE);
  tft.fillCircle(x + 12, y + 4, 2, COLOR_WHITE);
}

void drawFullMenu(int selected) {
  tft.fillScreen(COLOR_BLACK); 
  tft.setTextSize(3);
  tft.setCursor(120, 10);
  tft.setTextColor(COLOR_TITLE);
  tft.print("SPIELAUSWAHL");
  tft.drawFastHLine(0, 45, 480, COLOR_LINE);
  for (int i = 0; i < totalGames; i++) {
    int yPos = 55 + (i * 25); 
    if (i == selected) { drawGhost(10, yPos, COLOR_GHOST); tft.setTextColor(COLOR_GHOST, COLOR_BLACK); }
    else { tft.setTextColor(COLOR_INACTIVE, COLOR_BLACK); }
    tft.setCursor(55, yPos); tft.print(gameNames[i]);
  }
}

void updateMenuCursor(int oldIdx, int newIdx) {
  tft.setTextSize(3);
  if (oldIdx >= 0) {
    int oldY = 55 + (oldIdx * 25);
    tft.fillRect(8, oldY - 5, 25, 25, COLOR_BLACK); 
    tft.setCursor(55, oldY); tft.setTextColor(COLOR_INACTIVE, COLOR_BLACK); tft.print(gameNames[oldIdx]);
  }
  int newY = 55 + (newIdx * 25);
  drawGhost(10, newY, COLOR_GHOST);
  tft.setCursor(55, newY); tft.setTextColor(COLOR_GHOST, COLOR_BLACK); tft.print(gameNames[newIdx]);
}

// --- 8. SPIEL-WEICHE ---
void startGame(int id) {
  tft.fillScreen(COLOR_BLACK);
  if (id == 0) playVierGewinnt(); else if (id == 1) playMasterDesaster(); 
  else if (id == 2) playSlotMaschine(); else if (id == 3) playTheDinoGame();
  else if (id == 4) playMillisGame(); else if (id == 5) playZahlenRaten();
  else if (id == 6) playKlickFit(); else if (id == 7) playWuerfel();
  else if (id == 8) playSimonSagt(); else if (id == 9) playBreakout(); 
  setMenuLeds(); encoderPos = (long)id * ENC_STEPS; lastMenuIndex = -1; lastActivityTime = millis(); tft.fillScreen(COLOR_BLACK);
}

// --- 9. SETUP ---
void setup() {
  pinMode(ENC_BTN, INPUT_PULLUP);
  pinMode(BTN_MENU, INPUT_PULLUP);
  pinMode(BTN_START, INPUT_PULLUP);
  pinMode(BTN_P1, INPUT_PULLUP);
  pinMode(BTN_P2, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  lastStateA = digitalRead(ENC_A);
  attachInterrupt(digitalPinToInterrupt(ENC_A), readEncoder, CHANGE);

  pixels.begin();
  setupLeds(); 
  
  RTC.begin();
  syncRTCtoCompileTime(); 
  randomSeed(analogRead(SEED_PIN));

  dht.begin(); 

  tft.setSpiKludge(false);
  tft.init();
  tft.setRotation(1); 
  tft.fillScreen(COLOR_BLACK); 

  if (digitalRead(BTN_START) == LOW || digitalRead(BTN_MENU) == LOW || digitalRead(ENC_BTN) == LOW) {
    runSystemTest(); 
    tft.fillScreen(COLOR_BLACK);
    encoderPos = 0;
    setMenuLeds(); 
  }
  lastActivityTime = millis();
}

// --- 10. HAUPTSCHLEIFE ---
void loop() {
  if (millis() - lastActivityTime > idleTimeout) {
    showAnalogClock();
  }

  if (digitalRead(BTN_START) == LOW) { delay(200); runCalculator(); lastActivityTime = millis(); lastMenuIndex = -1; tft.fillScreen(COLOR_BLACK); }
  if (digitalRead(BTN_P1) == LOW) { delay(200); runSetupMenu(); lastActivityTime = millis(); lastMenuIndex = -1; tft.fillScreen(COLOR_BLACK); }
  if (digitalRead(BTN_P2) == LOW) { delay(200); runSmileyBoost(); lastActivityTime = millis(); lastMenuIndex = -1; tft.fillScreen(COLOR_BLACK); }
  if (digitalRead(BTN_MENU) == LOW) { lastActivityTime = millis(); }

  int sel = (encoderPos / ENC_STEPS) % totalGames;
  if (sel < 0) sel += totalGames;

  if (sel != lastMenuIndex) {
    if (lastMenuIndex == -1) drawFullMenu(sel); 
    else updateMenuCursor(lastMenuIndex, sel); 
    lastMenuIndex = sel;
  }

  if (digitalRead(ENC_BTN) == LOW) { delay(250); lastActivityTime = millis(); startGame(sel); }
}
