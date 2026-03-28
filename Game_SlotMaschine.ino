// --- PROTOTYP ---
void playSlotMaschine();

// --- LOKALE FARBEN FÜR DEN VEGAS-LOOK ---
#define SM_BLACK     0x0000
#define SM_BG_CYAN   0x07FF
#define SM_DEEP_BLUE 0x0018
#define SM_NEON_PINK 0xF81F
#define SM_NEON_BLUE 0x04FF
#define SM_GOLD      0xFDA0
#define SM_ORANGE    0xFD20
#define SM_BRIGHT_RED 0xF800
#define SM_SILVER    0xC618
#define SM_LIGHT_GREY 0xAD55
#define SM_SYM_GREEN 0x07E0

int smLastShapes[3];

// --- GRAFIK-FUNKTIONEN ---

void smDrawTriangle(int x, int y, uint16_t color) {
  tft.fillTriangle(x, y - 15, x - 15, y + 13, x + 15, y + 13, color);
}

void smDrawCircle(int x, int y, uint16_t color) {
  tft.fillCircle(x, y, 14, color);
}

void smDrawStar(int x, int y, uint16_t color) {
  float radius = 15;
  for (int i = 0; i < 5; i++) {
    float angle = i * 2 * PI / 5 - PI / 2;
    float nextAngle = (i + 2) * 2 * PI / 5 - PI / 2;
    tft.fillTriangle(x, y, x + cos(angle) * radius, y + sin(angle) * radius,
                     x + cos(nextAngle) * radius, y + sin(nextAngle) * radius, color);
  }
}

void smDrawRandomShape(int x, int y, int shapeID) {
  tft.fillRect(x - 40, y - 20, 80, 40, SM_BG_CYAN);
  if (shapeID == 0) smDrawTriangle(x, y, SM_SYM_GREEN);
  else if (shapeID == 1) smDrawCircle(x, y, SM_BRIGHT_RED);
  else if (shapeID == 2) smDrawStar(x, y, 0x001F);
}

void smDrawNeonFrame(int x, int y, int w, int h, int r, uint16_t c1, uint16_t c2, uint16_t c3) {
  tft.drawRoundRect(x, y, w, h, r, c1);
  tft.drawRoundRect(x + 1, y + 1, w - 2, h - 2, r, c2);
  tft.drawRoundRect(x + 2, y + 2, w - 4, h - 4, r, c3);
}

void smDrawLever(int yKugel) {
  int xHebel = 460;
  tft.fillRect(425, 60, 55, 245, SM_DEEP_BLUE);
  tft.fillRect(xHebel - 6, 105, 12, 140, SM_BLACK);
  tft.drawRect(xHebel - 7, 104, 14, 142, SM_SILVER);

  int rodHeight = abs(yKugel - 200) + 5;
  int rodTop = min(yKugel, 200);
  tft.fillRect(xHebel - 4, rodTop, 8, rodHeight, SM_LIGHT_GREY);
  tft.drawFastVLine(xHebel - 1, rodTop, rodHeight, COLOR_WHITE);

  tft.fillCircle(xHebel, yKugel, 15, SM_BRIGHT_RED);
  tft.fillCircle(xHebel - 5, yKugel - 5, 4, COLOR_WHITE);

  tft.fillRoundRect(415, 190, 15, 25, 4, SM_SILVER);
}

void smDrawBase() {
  tft.fillScreen(SM_DEEP_BLUE);
  tft.fillRoundRect(15, 30, 405, 285, 20, SM_BLACK);
  smDrawNeonFrame(15, 30, 405, 285, 20, SM_NEON_BLUE, SM_NEON_PINK, COLOR_WHITE);

  tft.fillRect(35, 45, 365, 50, SM_DEEP_BLUE);
  smDrawNeonFrame(35, 45, 365, 50, 10, SM_GOLD, SM_ORANGE, SM_GOLD);

  tft.setCursor(55, 55); tft.setTextSize(4);
  tft.setTextColor(SM_NEON_BLUE); tft.print("SLOT MACHINE");
  tft.setCursor(52, 52); tft.setTextColor(SM_GOLD); tft.print("SLOT MACHINE");

  for (int i = 0; i < 9; i++) {
    tft.fillCircle(50 + (i * 40), 110, 8, (i % 2 == 0 ? SM_ORANGE : SM_BRIGHT_RED));
    tft.fillCircle(50 + (i * 40), 290, 8, (i % 2 == 0 ? SM_BRIGHT_RED : SM_ORANGE));
  }

  smDrawNeonFrame(30, 130, 375, 145, 10, SM_ORANGE, SM_GOLD, COLOR_WHITE);

  int yWalze = 135; int hWalze = 135;
  tft.fillRect(45, yWalze, 95, hWalze, SM_BG_CYAN);
  tft.fillRect(172, yWalze, 95, hWalze, SM_BG_CYAN);
  tft.fillRect(300, yWalze, 95, hWalze, SM_BG_CYAN);
  
  smDrawLever(120);
}

void smRollSlots() {
  int xCenters[] = {92, 219, 347};
  for (int col = 0; col < 3; col++) {
    smDrawRandomShape(xCenters[col], 162, random(0, 3));
    smDrawRandomShape(xCenters[col], 244, random(0, 3));
    int shapeID = random(0, 3);
    smDrawRandomShape(xCenters[col], 203, shapeID);
    smLastShapes[col] = shapeID;
  }
}

void smShowScore() {
  int result = 1;
  if (smLastShapes[0] == smLastShapes[1] && smLastShapes[1] == smLastShapes[2]) result = 3;
  else if (smLastShapes[0] == smLastShapes[1] || smLastShapes[1] == smLastShapes[2] || smLastShapes[0] == smLastShapes[2]) result = 2;

  tft.fillRect(430, 10, 45, 45, SM_DEEP_BLUE);
  smDrawNeonFrame(430, 10, 45, 45, 5, SM_NEON_BLUE, SM_NEON_PINK, COLOR_WHITE);
  tft.setCursor(438, 18); tft.setTextColor(SM_GOLD); tft.setTextSize(4);
  tft.print(result);
}

// --- HAUPTFUNKTION ---
void playSlotMaschine() {
  smDrawBase();
  
  while (true) {
    if (digitalRead(BTN_MENU) == LOW) return; // Zurück zum Geist

    if (digitalRead(BTN_START) == LOW) { // Start mit A1
      tft.fillRect(430, 10, 45, 45, SM_DEEP_BLUE);
      smDrawNeonFrame(430, 10, 45, 45, 5, SM_NEON_BLUE, SM_NEON_PINK, COLOR_WHITE);
      
      // Hebel-Animation abwärts
      for (int y = 120; y <= 240; y += 40) { smDrawLever(y); delay(15); }
      
      // Walzen drehen
      for (int r = 0; r < 12; r++) { 
        smRollSlots(); 
        delay(20 + (r * 20)); 
      }
      
      smShowScore();
      
      // Hebel-Animation aufwärts
      for (int y = 240; y >= 120; y -= 40) { smDrawLever(y); delay(15); }
      delay(400);
    }
    delay(10);
  }
}