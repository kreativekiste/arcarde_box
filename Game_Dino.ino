// --- PROTOTYP ---
void playTheDinoGame();

// --- VARIABLEN FÜR DAS DINO GAME ---
float dinoY;
float dinoVelocity;
bool dinoIsJumping;
int dinoScore;
int dinoHighScore = 0;

float obsX;
int obsW = 20; // Kaktus-Breite
int obsH = 30; // Kaktus-Höhe
float obsSpeed;

// Merker für die Anti-Flacker-Logik
int dinoOldY;
int obsOldX;

// Spiel-Zustand
bool dinoGameRunning = false; // NEU: Steuert den Startbildschirm

// Konstanten für das Layout
#define DINO_X       60
#define GROUND_Y     260
#define DINO_SIZE    24
#define GRAVITY      1.2
#define JUMP_POWER   -14.0

// --- GRAFIK FUNKTIONEN ---

void dinoDrawPlayer(int y, uint16_t color) {
  // Simpler Pixel Dino
  tft.fillRect(DINO_X, y, DINO_SIZE, DINO_SIZE, color);
  if (color != COLOR_BLACK) {
    tft.fillRect(DINO_X + 16, y + 4, 4, 4, COLOR_BLACK); // Auge
    tft.fillRect(DINO_X + 16, y + 12, 8, 4, COLOR_BLACK); // Maul
  }
}

void dinoDrawObs(int x, uint16_t color) {
  int drawX = x;
  int drawW = obsW;

  if (drawX < 0) {
    drawW = obsW + drawX; 
    drawX = 0;            
  }
  
  if (drawW > 0) {
    tft.fillRect(drawX, GROUND_Y - obsH, drawW, obsH, color);
  }
}

// --- SPIEL LOGIK ---

void dinoShowStartScreen() {
  tft.fillScreen(COLOR_BLACK);
  
  // Boden zeichnen
  tft.drawFastHLine(0, GROUND_Y, 480, COLOR_LINE);
  tft.drawFastHLine(0, GROUND_Y + 2, 480, COLOR_LINE); 
  
  dinoDrawPlayer(GROUND_Y - DINO_SIZE, 0x07E0); // Wartender Dino
  
  // Titel & Start-Aufforderung
  tft.setTextSize(4);
  tft.setTextColor(0x07FF); 
  tft.setCursor(140, 80); 
  tft.print("DINO RUN");
  
  tft.setTextSize(2);
  tft.setTextColor(COLOR_WHITE);
  tft.setCursor(140, 150); 
  tft.print("PRESS START (A1)");
  
  tft.setCursor(320, 20); 
  tft.print("HIGH: "); tft.print(dinoHighScore);
  
  dinoGameRunning = false; // Spiel wartet auf den Spieler
}

void dinoReset() {
  dinoY = GROUND_Y - DINO_SIZE;
  dinoOldY = dinoY;
  dinoVelocity = 0;
  dinoIsJumping = false;
  dinoScore = 0;
  
  obsX = 480;
  obsSpeed = 6.0; 
  obsH = random(25, 55); 
  obsOldX = obsX;
  
  tft.fillScreen(COLOR_BLACK);
  tft.drawFastHLine(0, GROUND_Y, 480, COLOR_LINE);
  tft.drawFastHLine(0, GROUND_Y + 2, 480, COLOR_LINE); 
  
  tft.setTextSize(2);
  tft.setTextColor(COLOR_WHITE);
  tft.setCursor(20, 20); tft.print("SCORE: 0");
  tft.setCursor(320, 20); tft.print("HIGH: "); tft.print(dinoHighScore);
  
  dinoGameRunning = true; // Action geht los!
}

// --- HAUPTFUNKTION ---

void playTheDinoGame() {
  dinoShowStartScreen(); // Zeigt zuerst den Wartemodus
  
  while(true) {
    // 1. Zurück zum Menü (A4)
    if (digitalRead(BTN_MENU) == LOW) return;

    // 2. WARTEMODUS (Nur Dino sichtbar)
    if (!dinoGameRunning) {
      if (digitalRead(BTN_START) == LOW) { // Start-Knopf gedrückt
        dinoReset();
        delay(200); // Entprellen
      }
      delay(10);
      continue; // Blockiert die Bewegung, bis das Spiel gestartet wird
    }

    // 3. SPIEL LÄUFT: Springen mit Player 1 (A2) oder Player 2 (A3)
    if ((digitalRead(BTN_P1) == LOW || digitalRead(BTN_P2) == LOW) && !dinoIsJumping) {
      dinoVelocity = JUMP_POWER; 
      dinoIsJumping = true;
    }

    // 4. PHYSIK BERECHNEN
    dinoVelocity += GRAVITY; 
    dinoY += dinoVelocity;

    // Bodenkollision
    if (dinoY >= GROUND_Y - DINO_SIZE) {
      dinoY = GROUND_Y - DINO_SIZE;
      dinoVelocity = 0;
      dinoIsJumping = false;
    }

    // 5. HINDERNIS BEWEGEN
    obsX -= obsSpeed;
    
    // Kaktus ist aus dem Bild -> Neu spawnen
    if (obsX < -obsW) {
      // Sicherheits-Löscher für den extremen linken Rand
      tft.fillRect(0, GROUND_Y - 60, obsW + 5, 60, COLOR_BLACK);
      
      obsX = 480 + random(0, 150); 
      obsSpeed += 0.3;             // Schneller werden
      if (obsSpeed > 15.0) obsSpeed = 15.0; 
      
      obsH = random(20, 50);       // Neue Zufallshöhe
      dinoScore++;
      
      // Punkte aktualisieren
      tft.fillRect(90, 20, 100, 20, COLOR_BLACK);
      tft.setCursor(20, 20); 
      tft.print("SCORE: "); tft.print(dinoScore);
    }

    // 6. KOLLISIONSABFRAGE
    bool hitX = (DINO_X < obsX + obsW - 4) && (DINO_X + DINO_SIZE - 4 > obsX); 
    bool hitY = (dinoY + DINO_SIZE > GROUND_Y - obsH);
    
    if (hitX && hitY) {
      // GAME OVER
      tft.setTextSize(5);
      tft.setTextColor(0xF800); 
      tft.setCursor(100, 120); 
      tft.print("GAME OVER");
      
      if (dinoScore > dinoHighScore) dinoHighScore = dinoScore;
      
      delay(3000); 
      dinoShowStartScreen(); // Nach Game Over zurück zum "Press Start"
      continue;
    }

    // 7. GRAFIK ZEICHNEN
    
    // Dino nur neu zeichnen, wenn er sich auf der Y-Achse bewegt hat
    if ((int)dinoY != dinoOldY) {
      dinoDrawPlayer(dinoOldY, COLOR_BLACK); 
      dinoDrawPlayer((int)dinoY, 0x07E0);    
      dinoOldY = (int)dinoY;
    } else {
      dinoDrawPlayer((int)dinoY, 0x07E0); 
    }

    // Kaktus bewegen & Schweif radieren
    if ((int)obsX != obsOldX) {
      int tailX = (int)obsX + obsW;
      int tailW = obsOldX - (int)obsX;
      
      if (tailX >= 0 && tailX < 480) {
        tft.fillRect(tailX, GROUND_Y - obsH, tailW, obsH, COLOR_BLACK);
      }
      
      dinoDrawObs((int)obsX, 0xF800); 
      obsOldX = (int)obsX;
    }

    delay(15); // Frame-Rate
  }
}