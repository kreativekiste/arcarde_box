// --- PROTOTYP ---
void playSimonSagt();

// --- HARDWARE SETUP (Konstanten) ---
const int simonBuzzer = 6; 
const int simonMaxRounds = 32;

// Frequenzen
const int noteRed = 440;    
const int noteGreen = 880;  
const int noteBlue = 587;   
const int noteYellow = 784; 

// Farben (RGB565)
const uint16_t cRed = 0xA800;   const uint16_t cRed_H = 0xF800;
const uint16_t cGreen = 0x0540; const uint16_t cGreen_H = 0x07E0;
const uint16_t cBlue = 0x0010;  const uint16_t cBlue_H = 0x001F;
const uint16_t cYellow = 0xABA0; const uint16_t cYellow_H = 0xFFE0;

// --- VARIABLEN ---
byte simonSequence[simonMaxRounds];
byte simonRound = 0;

// --- GRAFIK & SOUND ---

void simonDrawButton(int choice, bool lit) {
  if (choice == 0) tft.fillRect(0, 0, 240, 160, lit ? cRed_H : cRed);       // A2 (Rot)
  if (choice == 1) tft.fillRect(240, 0, 240, 160, lit ? cGreen_H : cGreen); // A3 (Grün)
  if (choice == 2) tft.fillRect(0, 160, 240, 160, lit ? cBlue_H : cBlue);   // A4 (Blau)
  if (choice == 3) tft.fillRect(240, 160, 240, 160, lit ? cYellow_H : cYellow); // A1 (Gelb)
  
  tft.drawFastHLine(0, 160, 480, COLOR_BLACK);
  tft.drawFastVLine(240, 0, 320, COLOR_BLACK);
}

void simonSignal(int choice, int duration) {
  int freq = 0;
  if (choice == 0) freq = noteRed;
  if (choice == 1) freq = noteGreen;
  if (choice == 2) freq = noteBlue;
  if (choice == 3) freq = noteYellow;

  simonDrawButton(choice, true);
  tone(simonBuzzer, freq);
  delay(duration);
  noTone(simonBuzzer);
  simonDrawButton(choice, false);
}

// Abfrage der 4 analogen Taster als Farben
int simonCheckButtons() {
  if (digitalRead(BTN_P1) == LOW) return 0;    // A2 -> ROT
  if (digitalRead(BTN_P2) == LOW) return 1;    // A3 -> GRÜN
  if (digitalRead(BTN_MENU) == LOW) return 2;  // A4 -> BLAU
  if (digitalRead(BTN_START) == LOW) return 3; // A1 -> GELB
  return -1;
}

void simonGameOver() {
  tone(simonBuzzer, 150, 800);
  tft.fillScreen(0xF800);
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(5);
  tft.setCursor(100, 120);
  tft.print("GAME OVER");
  tft.setTextSize(3);
  tft.setCursor(140, 200);
  tft.print("SCORE: "); tft.print(simonRound - 1);
  delay(3000);
}

// --- HAUPTSCHLEIFE ---

void playSimonSagt() {
  pinMode(simonBuzzer, OUTPUT);
  bool gameActive = true;

  while (gameActive) {
    tft.fillScreen(COLOR_BLACK);
    for (int i = 0; i < 4; i++) simonDrawButton(i, false);
    
    simonRound = 0;
    bool playerLost = false;
    delay(1000);

    while (simonRound < simonMaxRounds && !playerLost) {
      simonSequence[simonRound] = random(0, 4);
      simonRound++;

      // 1. Sequenz vorspielen
      for (int i = 0; i < simonRound; i++) {
        // EXIT: Encoder-Druck (Pin 4) führt zurück zum Menü
        if (digitalRead(ENC_BTN) == LOW) return; 
        simonSignal(simonSequence[i], 400);
        delay(200);
      }

      // 2. Spieler-Eingabe
      for (int i = 0; i < simonRound; i++) {
        unsigned long startTime = millis();
        int playerChoice = -1;

        while (playerChoice == -1) {
          if (digitalRead(ENC_BTN) == LOW) return; // EXIT
          if (millis() - startTime > 4000) { playerLost = true; break; }
          playerChoice = simonCheckButtons();
        }

        if (playerLost) break;

        simonSignal(playerChoice, 250);
        while (simonCheckButtons() != -1); // Warten auf Loslassen
        delay(100);

        if (playerChoice != simonSequence[i]) {
          playerLost = true;
          break;
        }
      }
      if (!playerLost) delay(600);
    }

    simonGameOver();
    
    tft.setTextSize(2);
    tft.setTextColor(COLOR_WHITE);
    tft.setCursor(50, 280);
    tft.print("START(A1):NEU | RAD DRUECKEN:ENDE");
    
    while(true) {
      if (digitalRead(ENC_BTN) == LOW) return;
      if (digitalRead(BTN_START) == LOW) break;
    }
  }
}