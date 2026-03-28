// --- PROTOTYP ---
void playKlickFit();

// --- KONSTANTEN & VARIABLEN ---
const int kfGoal = 50; 
int kfCountP1 = 0;
int kfCountP2 = 0;
bool kfRunning = false;

const int kfBarX = 40;
const int kfBarW = 400; 
const int kfBarH = 40;
const int kfP1Y = 100;
const int kfP2Y = 200;

// Farben für dieses Spiel
#define KF_RED   0xF800
#define KF_GREEN 0x07E0

// --- HILFSFUNKTIONEN ---

void kfDrawIcon(int x, int y, uint16_t color) {
  tft.fillCircle(x, y + 20, 15, color);
  tft.fillTriangle(x, y + 20, x + 18, y + 5, x + 18, y + 35, COLOR_BLACK);
}

void kfDrawBase() {
  tft.fillScreen(COLOR_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TITLE);
  tft.setCursor(150, 20);
  tft.print("KLICK FIT!");
  
  tft.drawFastHLine(0, 60, 480, COLOR_LINE);

  // Rahmen zeichnen
  tft.drawRect(kfBarX - 2, kfP1Y - 2, kfBarW + 4, kfBarH + 4, COLOR_WHITE);
  tft.drawRect(kfBarX - 2, kfP2Y - 2, kfBarW + 4, kfBarH + 4, COLOR_WHITE);
  
  tft.setTextSize(2);
  tft.setTextColor(COLOR_WHITE);
  tft.setCursor(kfBarX, kfP1Y - 25); tft.print("SPIELER 1 (A2)");
  tft.setCursor(kfBarX, kfP2Y - 25); tft.print("SPIELER 2 (A3)");

  // START-ICONS: Oben Rot, Unten Grün
  kfDrawIcon(kfBarX, kfP1Y, KF_RED); 
  kfDrawIcon(kfBarX, kfP2Y, KF_GREEN);
}

void kfShowResult(int winner) {
  kfRunning = false;
  tft.setTextSize(4);
  tft.setCursor(100, 150);
  
  if (winner == 1) {
    tft.setTextColor(KF_RED);
    tft.print("P1 GEWINNT!");
    tft.fillRect(kfBarX, kfP2Y, kfBarW, kfBarH, COLOR_BLACK); // P2 löschen
  } else {
    tft.setTextColor(KF_GREEN);
    tft.print("P2 GEWINNT!");
    tft.fillRect(kfBarX, kfP1Y, kfBarW, kfBarH, COLOR_BLACK); // P1 löschen
  }

  tft.setTextSize(2);
  tft.setTextColor(COLOR_INACTIVE);
  tft.setCursor(80, 280);
  tft.print("START-KNOPF ZUM RESET / NEUSTART");
}

void playKlickFit() {
  kfDrawBase();
  kfCountP1 = 0;
  kfCountP2 = 0;
  kfRunning = false;

  bool lastP1 = HIGH;
  bool lastP2 = HIGH;

  while (true) {
    if (digitalRead(BTN_MENU) == LOW) return;

    if (!kfRunning) {
      if (digitalRead(BTN_START) == LOW) {
        kfCountP1 = 0;
        kfCountP2 = 0;
        kfDrawBase();
        kfRunning = true;
        delay(200);
      }
      continue; 
    }

    bool currentP1 = digitalRead(BTN_P1);
    bool currentP2 = digitalRead(BTN_P2);

    // Spieler 1 (ROT)
    if (currentP1 == LOW && lastP1 == HIGH) {
      kfCountP1++;
      int progressW = kfCountP1 * (kfBarW / kfGoal);
      tft.fillRect(kfBarX, kfP1Y, progressW, kfBarH, KF_RED); 
      kfDrawIcon(kfBarX + progressW, kfP1Y, KF_RED);
      
      if (kfCountP1 >= kfGoal) kfShowResult(1);
    }
    lastP1 = currentP1;

    // Spieler 2 (GRUEN)
    if (currentP2 == LOW && lastP2 == HIGH) {
      kfCountP2++;
      int progressW = kfCountP2 * (kfBarW / kfGoal);
      tft.fillRect(kfBarX, kfP2Y, progressW, kfBarH, KF_GREEN); 
      kfDrawIcon(kfBarX + progressW, kfP2Y, KF_GREEN);
      
      if (kfCountP2 >= kfGoal) kfShowResult(2);
    }
    lastP2 = currentP2;

    delay(5); 
  }
}