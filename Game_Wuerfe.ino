// --- PROTOTYP ---
void playWuerfel();

// --- KONSTANTEN & VARIABLEN ---
const int wMaxDice = 5;
int wSelectedCount = 1;
int wResults[5];
bool wHasRolled = false;

// --- HILFSFUNKTION: WÜRFEL MIT AUGEN ZEICHNEN ---
// x, y = Mitte des Würfels, val = Wert (1-6), size = Kantenlänge
void drawDicePips(int x, int y, int val, int size) {
  // Würfelkörper
  tft.fillRect(x - (size/2), y - (size/2), size, size, COLOR_WHITE);
  tft.drawRect(x - (size/2), y - (size/2), size, size, COLOR_BLACK);
  
  int r = size / 12; // Punktgröße skaliert mit Würfelgröße
  int d = size / 4;  // Abstand der Punkte von der Mitte

  // Lambda-Funktion zum Zeichnen eines Punktes
  auto p = [&](int dx, int dy) { 
    tft.fillCircle(x + dx, y + dy, r, COLOR_BLACK); 
  };

  // Logik für die Augen-Anordnung
  if (val == 1 || val == 3 || val == 5) p(0, 0); // Mitte
  if (val > 1) { p(-d, -d); p(d, d); }           // Oben links, unten rechts
  if (val > 3) { p(d, -d); p(-d, d); }           // Oben rechts, unten links
  if (val == 6) { p(-d, 0); p(d, 0); }           // Mitte links/rechts
}

// Zeichnet alle gewürfelten Augen nebeneinander
void displayAllDice() {
  // Größe anpassen: 1 Würfel groß, ab 2 Würfeln kleiner
  int size = (wSelectedCount == 1) ? 140 : (wSelectedCount > 3 ? 75 : 90);
  int spacing = 15;
  int totalW = (wSelectedCount * size) + ((wSelectedCount - 1) * spacing);
  int startX = (480 - totalW) / 2 + (size / 2); // Start-X (Mitte des ersten Würfels)
  int yPos = 160;

  for (int i = 0; i < wSelectedCount; i++) {
    drawDicePips(startX + (i * (size + spacing)), yPos, wResults[i], size);
  }
}

void drawStaticUI() {
  tft.fillScreen(COLOR_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(COLOR_TITLE);
  tft.setCursor(160, 15);
  tft.print("WUERFELN");
  tft.drawFastHLine(0, 50, 480, COLOR_LINE);

  tft.setTextSize(2);
  tft.setTextColor(COLOR_GHOST);
  tft.setCursor(80, 80);
  tft.print("WIE VIELE WUERFEL?");

  tft.setTextColor(COLOR_INACTIVE);
  tft.setCursor(100, 280);
  tft.print("DRUECKE START (A2) ZUM WUERFELN");
}

void playWuerfel() {
  drawStaticUI();
  wSelectedCount = 1;
  wHasRolled = false;
  int lastSelected = -1;

  while (true) {
    if (digitalRead(BTN_MENU) == LOW) return;

    if (!wHasRolled) {
      // Auswahl der Anzahl (1-5)
      int count = abs((encoderPos / ENC_STEPS) % 5) + 1;
      
      if (count != lastSelected) {
        tft.setTextSize(10);
        tft.setTextColor(COLOR_WHITE, COLOR_BLACK); 
        tft.setCursor(215, 130);
        tft.print(count);
        lastSelected = count;
        wSelectedCount = count;
      }

      if (digitalRead(BTN_P1) == LOW) { 
        wHasRolled = true;
        tft.fillRect(0, 60, 480, 215, COLOR_BLACK); 
        delay(200);
      }
    } 
    else {
      // Würfel-Animation (Rollen)
      for (int a = 0; a < 12; a++) {
        for (int i = 0; i < wSelectedCount; i++) wResults[i] = random(1, 7);
        displayAllDice();
        delay(30 + (a * 10)); 
      }
      
      tft.setTextSize(2);
      tft.setTextColor(COLOR_INACTIVE, COLOR_BLACK);
      tft.setCursor(110, 280);
      tft.print("A2: ERNEUT | RAD: ANZAHL "); 
      
      while (true) {
        if (digitalRead(BTN_MENU) == LOW) return;
        if (digitalRead(BTN_P1) == LOW) break; 
        
        int checkCount = abs((encoderPos / ENC_STEPS) % 5) + 1;
        if (checkCount != wSelectedCount) {
          wHasRolled = false;
          drawStaticUI(); 
          lastSelected = -1;
          break;
        }
      }
      delay(200);
    }
  }
}