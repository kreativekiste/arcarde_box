// --- PROTOTYP ---
void playMasterDesaster();

// --- VARIABLEN FÜR MASTER DESASTER ---
uint16_t mmColors[] = {0xF81F, 0x001F, 0xF800, 0x07E0, 0xFFE0, 0x07FF}; // 6 Neon-Farben
int mmSecretCode[4];
int mmCurrentGuess[4];

// Historie für bis zu 45 Versuche
byte mmHistory[45][4];
byte mmHistoryA[45]; // Richtige Farbe & Platz (Treffer)
byte mmHistoryB[45]; // Nur richtige Farbe (Farben)

int mmAttempt = 0;
int mmCurrentPos = 0;

// Anti-Flacker-Variablen
int mmLastColorIdx = -1;
bool mmNeedsInputRedraw = true;

// --- NEUES LAYOUT FÜR QUERFORMAT ---
#define MM_HIST_X   20   // Start X für die Historie (etwas weiter links für mehr Platz)
#define MM_HIST_Y   60   // Start Y für die oberste Zeile
#define MM_HIST_SP  48   // Zeilenabstand (so passen 5 Stück exakt übereinander)
#define MM_INPUT_X  290  // Eingabebereich weit nach rechts geschoben
#define MM_INPUT_Y  140  // Eingabebereich mittig auf der Y-Achse
#define MM_BOX_SIZE 30   // Einheitliche Steinchen-Größe für Historie und Eingabe (kleiner!)

// --- HILFSFUNKTIONEN ---

void mmDrawUI() {
  tft.fillRect(0, 0, 480, 50, 0x2104);
  tft.setTextSize(2);
  tft.setTextColor(COLOR_WHITE);

  tft.setCursor(20, 15);
  tft.print("VERSUCH: "); tft.print(mmAttempt + 1);
  tft.setCursor(330, 15);
  tft.print("REST: "); tft.print(45 - mmAttempt);
  tft.drawFastHLine(0, 50, 480, COLOR_LINE);
}

void mmDrawShape(int x, int y, uint16_t color, bool selected, int size) {
  if (selected) tft.drawRect(x - 3, y - 3, size + 6, size + 6, COLOR_WHITE);
  else tft.drawRect(x - 3, y - 3, size + 6, size + 6, COLOR_BLACK);
  tft.fillRect(x, y, size, size, color);
}

void mmDrawInputFrame() {
  tft.fillRect(MM_INPUT_X - 15, MM_INPUT_Y - 20, 190, 80, COLOR_BLACK); 
  tft.drawRect(MM_INPUT_X - 10, MM_INPUT_Y - 15, 180, 70, 0x2104);
  tft.setTextSize(1);
  tft.setTextColor(0x07FF);
  tft.setCursor(MM_INPUT_X - 5, MM_INPUT_Y - 10);
  tft.print("EINGABE:");
}

void mmReset() {
  mmAttempt = 0;
  mmCurrentPos = 0;
  
  // Zufallscode generieren
  for(int i = 0; i < 4; i++) {
    mmSecretCode[i] = random(0, 6); 
    mmCurrentGuess[i] = 0;
  }
  
  // Historie löschen
  for (int i = 0; i < 45; i++) {
    mmHistoryA[i] = 0; mmHistoryB[i] = 0;
    for (int j = 0; j < 4; j++) mmHistory[i][j] = 0;
  }
  
  tft.fillScreen(COLOR_BLACK);
  mmDrawUI();
  mmDrawInputFrame();
  mmNeedsInputRedraw = true;
}

void mmDrawBoard() {
  // Zeige maximal die letzten 5 Versuche untereinander an
  int startIdx = (mmAttempt > 4) ? (mmAttempt - 5) : 0;
  
  tft.fillRect(0, 51, MM_INPUT_X - 20, 269, COLOR_BLACK); // Historien-Bereich links leeren
  
  for (int i = 0; i < 5; i++) {
    int rowIdx = startIdx + i;
    if (rowIdx >= mmAttempt) break;
    
    int y = MM_HIST_Y + (i * MM_HIST_SP);
    
    // 4 Steinchen zeichnen
    for (int p = 0; p < 4; p++) {
      mmDrawShape(MM_HIST_X + (p * 40), y, mmColors[mmHistory[rowIdx][p]], false, MM_BOX_SIZE);
    }
    
    // Texte daneben zeichnen
    tft.setTextSize(1);
    tft.setTextColor(0x07FF); tft.setCursor(MM_HIST_X + 170, y + 2);
    tft.print("TREFFER:"); tft.print(mmHistoryA[rowIdx]);
    tft.setTextColor(0xFDA0); tft.setCursor(MM_HIST_X + 170, y + 17);
    tft.print("FARBEN :"); tft.print(mmHistoryB[rowIdx]);
  }
}

bool mmCheckGuess() {
  int a = 0; int b = 0;
  bool sUsed[4] = {false}; bool gUsed[4] = {false};
  
  for (int i = 0; i < 4; i++) {
    if (mmCurrentGuess[i] == mmSecretCode[i]) { a++; sUsed[i] = gUsed[i] = true; }
  }
  for (int i = 0; i < 4; i++) {
    if (!gUsed[i]) {
      for (int j = 0; j < 4; j++) {
        if (!sUsed[j] && mmCurrentGuess[i] == mmSecretCode[j]) { b++; sUsed[j] = true; break; }
      }
    }
  }

  for (int i = 0; i < 4; i++) mmHistory[mmAttempt][i] = mmCurrentGuess[i];
  mmHistoryA[mmAttempt] = a;
  mmHistoryB[mmAttempt] = b;

  if (a == 4) {
    tft.fillScreen(0x07E0);
    tft.setTextColor(COLOR_BLACK); tft.setTextSize(4);
    tft.setCursor(120, 140); tft.print("GEWONNEN!");
    delay(4000);
    return true; 
  } 
  else if (mmAttempt >= 44) { 
    tft.fillScreen(0xF800);
    tft.setTextColor(COLOR_WHITE); tft.setTextSize(3);
    tft.setCursor(50, 100); tft.print("GAME OVER - CODE WAR:");
    for(int i=0; i<4; i++) {
      mmDrawShape(100 + (i * 60), 160, mmColors[mmSecretCode[i]], false, 45);
    }
    delay(5000);
    return true;
  }
  return false; 
}

// --- HAUPTFUNKTION ---
void playMasterDesaster() {
  mmReset();

  while (true) {
    if (digitalRead(BTN_MENU) == LOW) return;

    long newPos = encoderPos / ENC_STEPS;
    int colorIdx = abs(newPos % 6);

    // EINGABEFELD ZEICHNEN
    if (colorIdx != mmLastColorIdx || mmNeedsInputRedraw) {
      for(int i = 0; i < 4; i++) {
        uint16_t c;
        bool isSelected = (i == mmCurrentPos);
        
        if (isSelected) c = mmColors[colorIdx];
        else if (i < mmCurrentPos) c = mmColors[mmCurrentGuess[i]];
        else c = COLOR_BLACK; 
        
        // Abstand von 40px, damit die 30px Boxen gut nebeneinander passen
        mmDrawShape(MM_INPUT_X + (i * 40), MM_INPUT_Y, c, isSelected, MM_BOX_SIZE);
      }
      mmLastColorIdx = colorIdx;
      mmNeedsInputRedraw = false;
    }

    if (digitalRead(ENC_BTN) == LOW) {
      delay(250); 
      mmCurrentGuess[mmCurrentPos] = colorIdx; 
      mmCurrentPos++;
      mmNeedsInputRedraw = true; 
      
      if (mmCurrentPos > 3) {
        if (mmCheckGuess()) {
          mmReset(); 
        } else {
          mmAttempt++; 
          mmCurrentPos = 0;
          mmDrawUI();    
          mmDrawBoard(); 
          mmDrawInputFrame(); // Eingabebereich säubern für nächste Runde
        }
      }
    }
    delay(10); 
  }
}