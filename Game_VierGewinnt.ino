// --- PROTOTYP --- V1.1
int board[7][6]; 
int activePlayer = 1;
int cursorCol = 3;
long vgOldPos = -999;
bool vgGameOver = false;

int scoreP1 = 0;
int scoreP2 = 0;

// Speichert die Koordinaten der 4 Sieger-Steine
int winC[4]; 
int winR[4];
unsigned long vgLastBlinkTime = 0;
bool vgBlinkState = false;

// Layout-Konstanten
const int VG_START_X = 100;
const int VG_START_Y = 40;
const int VG_SLOT_SIZE = 40;
const int VG_RADIUS = 17;

// --- FUNKTIONEN ---
void vgDrawScores() {
  tft.fillRect(10, 130, 80, 60, COLOR_BLACK);
  tft.fillRect(390, 130, 80, 60, COLOR_BLACK);
  tft.setTextSize(7);
  tft.setTextColor(0xF800); // Rot
  tft.setCursor(30, 130); tft.print(scoreP1);
  tft.setTextColor(0x001F); // Blau
  tft.setCursor(410, 130); tft.print(scoreP2);
}

void vgDrawSlot(int c, int r, uint16_t color, bool filled) {
  int x = VG_START_X + c * VG_SLOT_SIZE + VG_SLOT_SIZE / 2;
  int y = VG_START_Y + (5 - r) * VG_SLOT_SIZE + VG_SLOT_SIZE / 2;
  if (filled) {
    tft.fillCircle(x, y, VG_RADIUS, color);
  } else {
    tft.fillCircle(x, y, VG_RADIUS, COLOR_BLACK); // Innenraum leeren
    tft.drawCircle(x, y, VG_RADIUS, color);       // Rand zeichnen
  }
}

void vgReset() {
  if (scoreP1 >= 5 || scoreP2 >= 5) { scoreP1 = 0; scoreP2 = 0; }
  tft.fillScreen(COLOR_BLACK);
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(2);
  tft.setCursor(144, 290); tft.print("kreativekiste.de");
  vgDrawScores();
  for (int c = 0; c < 7; c++) {
    for (int r = 0; r < 6; r++) {
      board[c][r] = 0;
      vgDrawSlot(c, r, COLOR_WHITE, false);
    }
  }
  vgGameOver = false;
  activePlayer = 1;
}

void vgDrawCursor() {
  tft.fillRect(90, 8, 300, 24, COLOR_BLACK); 
  int x = VG_START_X + cursorCol * VG_SLOT_SIZE + VG_SLOT_SIZE / 2;
  uint16_t col = (activePlayer == 1) ? 0xF800 : 0x001F;
  tft.fillCircle(x, 20, 8, col); 
}

bool vgCheckWin(int p) {
  // 1. Horizontal (-)
  for (int r = 0; r < 6; r++) {
    for (int c = 0; c < 4; c++) {
      if (board[c][r] == p && board[c+1][r] == p && board[c+2][r] == p && board[c+3][r] == p) {
        winC[0]=c; winR[0]=r; winC[1]=c+1; winR[1]=r; winC[2]=c+2; winR[2]=r; winC[3]=c+3; winR[3]=r;
        return true;
      }
    }
  }
  
  // 2. Vertikal (|)
  for (int c = 0; c < 7; c++) {
    for (int r = 0; r < 3; r++) {
      if (board[c][r] == p && board[c][r+1] == p && board[c][r+2] == p && board[c][r+3] == p) {
        winC[0]=c; winR[0]=r; winC[1]=c; winR[1]=r+1; winC[2]=c; winR[2]=r+2; winC[3]=c; winR[3]=r+3;
        return true;
      }
    }
  }
  
  // 3. Diagonal von Unten-Links nach Oben-Rechts (/)
  for (int c = 0; c < 4; c++) {
    for (int r = 0; r < 3; r++) {
      if (board[c][r] == p && board[c+1][r+1] == p && board[c+2][r+2] == p && board[c+3][r+3] == p) {
        winC[0]=c; winR[0]=r; winC[1]=c+1; winR[1]=r+1; winC[2]=c+2; winR[2]=r+2; winC[3]=c+3; winR[3]=r+3;
        return true;
      }
    }
  }
  
  // 4. Diagonal von Oben-Links nach Unten-Rechts (\)
  for (int c = 0; c < 4; c++) {
    for (int r = 3; r < 6; r++) {
      if (board[c][r] == p && board[c+1][r-1] == p && board[c+2][r-2] == p && board[c+3][r-3] == p) {
        winC[0]=c; winR[0]=r; winC[1]=c+1; winR[1]=r-1; winC[2]=c+2; winR[2]=r-2; winC[3]=c+3; winR[3]=r-3;
        return true;
      }
    }
  }
  return false;
}

void vgDrop() {
  for (int r = 0; r < 6; r++) {
    if (board[cursorCol][r] == 0) {
      board[cursorCol][r] = activePlayer;
      vgDrawSlot(cursorCol, r, (activePlayer == 1) ? 0xF800 : 0x001F, true);
      
      if (vgCheckWin(activePlayer)) {
        if (activePlayer == 1) scoreP1++; else scoreP2++;
        vgDrawScores();
        // Hier kein delay() mehr! Wir starten nur den Blink-Modus.
        vgGameOver = true;
        vgLastBlinkTime = millis();
        vgBlinkState = true;
      } else {
        activePlayer = (activePlayer == 1) ? 2 : 1;
        vgDrawCursor();
      }
      break;
    }
  }
}

// DAS HAUPT-ENTRY-POINT FÜR DAS SPIEL
void playVierGewinnt() {
  vgReset();
  
  while (true) {
    // 1. ZURÜCK ZUM MENÜ (Taster A4)
    if (digitalRead(BTN_MENU) == LOW) return;

    // 2. SPIEL-LOGIK (Eingabe)
    if (digitalRead(ENC_BTN) == LOW) {
      if (vgGameOver) {
        vgReset(); // Ein einziger Druck startet sofort neu!
      } else {
        vgDrop();
      }
      delay(250); // Entprellen
    }

    // 3. ANIMATION & BEWEGUNG
    if (!vgGameOver) {
      // Globaler Interrupt-Encoder
      long newPos = encoderPos / ENC_STEPS; 
      if (newPos != vgOldPos) {
        if (newPos > vgOldPos) cursorCol++; else cursorCol--;
        if (cursorCol < 0) cursorCol = 0;
        if (cursorCol > 6) cursorCol = 6;
        vgDrawCursor();
        vgOldPos = newPos;
      }
    } else {
      // Endlos-Blinken im Hintergrund (blockiert nicht!)
      if (millis() - vgLastBlinkTime > 300) {
        vgLastBlinkTime = millis();
        vgBlinkState = !vgBlinkState;
        uint16_t color = (activePlayer == 1) ? 0xF800 : 0x001F;
        
        for (int i = 0; i < 4; i++) {
          vgDrawSlot(winC[i], winR[i], vgBlinkState ? color : COLOR_WHITE, vgBlinkState);
        }
        
        if (vgBlinkState) playBuzzer(800, 50);
        else playBuzzer(400, 50);
      }
    }
    
    delay(10);
  }
}
