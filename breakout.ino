void playBreakout() {
  bool playAgain = true;

  while (playAgain) {
    tft.fillScreen(COLOR_BLACK);
    
    // High Piep beim Start!
    playBuzzer(1500, 100);
    delay(150);
    playBuzzer(2000, 150);

    // Spielfeld Konstanten
    const int PADDLE_W = 60;
    const int PADDLE_H = 10;
    const int PADDLE_Y = 300;
    const int BALL_R = 5;
    const int BRICK_ROWS = 4;
    const int BRICK_COLS = 8;
    const int BRICK_W = 56;
    const int BRICK_H = 20;

    // Variablen
    int paddleX = 210; // Start exakt in der Mitte
    int oldPaddleX = paddleX;
    
    float ballX = 240;
    float ballY = 200;
    float oldBallX = ballX;
    float oldBallY = ballY;
    float dX = 3.5;
    float dY = -3.5;

    bool bricks[BRICK_ROWS][BRICK_COLS];
    int bricksLeft = BRICK_ROWS * BRICK_COLS;

    // Level aufbauen
    for (int r = 0; r < BRICK_ROWS; r++) {
      for (int c = 0; c < BRICK_COLS; c++) {
        bricks[r][c] = true;
        uint16_t bColor = (r % 2 == 0) ? COLOR_TITLE : COLOR_GHOST;
        tft.fillRect(10 + c * (BRICK_W + 2), 30 + r * (BRICK_H + 2), BRICK_W, BRICK_H, bColor);
      }
    }

    // FIX 1: Paddle und Ball sofort zu Beginn einmal zeichnen!
    tft.fillRect(paddleX, PADDLE_Y, PADDLE_W, PADDLE_H, COLOR_WHITE);
    tft.fillCircle((int)ballX, (int)ballY, BALL_R, 0xF800);

    // Relative Encoder-Steuerung
    long lastGameEnc = encoderPos / ENC_STEPS;
    bool gameOver = false;
    bool gameWon = false;

    // Entprellen des Start-Knopfs (falls man ihn vom Restart noch gedrückt hält)
    while(digitalRead(ENC_BTN) == LOW) { delay(10); }

    while (!gameOver && !gameWon) {
      // Notausgang
      if (digitalRead(BTN_MENU) == LOW) return;

      // 1. Paddle bewegen (Relativ)
      long currentEnc = encoderPos / ENC_STEPS;
      long diff = currentEnc - lastGameEnc;

      if (diff != 0) {
        paddleX += diff * 22; // Hin und her Geschwindigkeit
        lastGameEnc = currentEnc;
      }

      // Begrenzung (Wände)
      if (paddleX < 0) paddleX = 0;
      if (paddleX > 480 - PADDLE_W) paddleX = 480 - PADDLE_W;

      if (paddleX != oldPaddleX) {
        tft.fillRect(oldPaddleX, PADDLE_Y, PADDLE_W, PADDLE_H, COLOR_BLACK);
        tft.fillRect(paddleX, PADDLE_Y, PADDLE_W, PADDLE_H, COLOR_WHITE);
        oldPaddleX = paddleX;
      }

      // 2. Ball bewegen
      ballX += dX;
      ballY += dY;

      // Wand Kollision
      if (ballX <= BALL_R || ballX >= 480 - BALL_R) dX = -dX;
      if (ballY <= BALL_R) dY = -dY;

      // Paddle Kollision (Mit leichter Toleranz am Rand des Paddles)
      if (ballY + BALL_R >= PADDLE_Y && ballY <= PADDLE_Y + PADDLE_H) {
        if (ballX >= paddleX - BALL_R && ballX <= paddleX + PADDLE_W + BALL_R) {
          dY = -dY;
          ballY = PADDLE_Y - BALL_R - 1; 
          playBuzzer(800, 20); // Kurzer Bounce-Ton
        }
      }

      // Boden erreicht = Game Over
      if (ballY >= 320) {
        gameOver = true;
      }

      // Brick Kollision 
      for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
          if (bricks[r][c]) {
            int bx = 10 + c * (BRICK_W + 2);
            int by = 30 + r * (BRICK_H + 2);
            
            if (ballX + BALL_R >= bx && ballX - BALL_R <= bx + BRICK_W && 
                ballY + BALL_R >= by && ballY - BALL_R <= by + BRICK_H) {
              
              bricks[r][c] = false;
              tft.fillRect(bx, by, BRICK_W, BRICK_H, COLOR_BLACK); 
              dY = -dY; 
              bricksLeft--;
              playBuzzer(1200, 30); // Hit-Ton
              
              r = BRICK_ROWS; 
              break;
            }
          }
        }
      }

      if (bricksLeft <= 0) gameWon = true;

      // Ball neu zeichnen
      if ((int)ballX != (int)oldBallX || (int)ballY != (int)oldBallY) {
        tft.fillCircle((int)oldBallX, (int)oldBallY, BALL_R, COLOR_BLACK);
        tft.fillCircle((int)ballX, (int)ballY, BALL_R, 0xF800);
        oldBallX = ballX;
        oldBallY = ballY;
      }

      delay(15); 
    }

    // --- SPIEL ENDE (GAME OVER ODER GEWONNEN) ---
    if (gameOver) {
      playBuzzer(300, 300); delay(350);
      playBuzzer(250, 300); delay(350);
      playBuzzer(150, 600);
      
      tft.setTextSize(4);
      tft.setTextColor(0xF800);
      tft.setCursor(120, 150);
      tft.print("GAME OVER");
    } else if (gameWon) {
      playBuzzer(800, 150); delay(150);
      playBuzzer(1000, 150); delay(150);
      playBuzzer(1500, 400);
      
      tft.setTextSize(4);
      tft.setTextColor(COLOR_GHOST);
      tft.setCursor(140, 150);
      tft.print("GEWONNEN!");
    }

    // FIX 2: RESTART MENÜ
    tft.setTextSize(2);
    tft.setTextColor(COLOR_WHITE);
    tft.setCursor(100, 220);
    tft.print("DRUECKE RAD = NEUSTART");
    tft.setCursor(130, 250);
    tft.print("DRUECKE GELB = ZUM MENUE");

    // Warten auf Entscheidung (Neustart oder Menü)
    while (true) {
      if (digitalRead(BTN_MENU) == LOW) {
        playAgain = false; // Beendet die äußere Schleife
        break; 
      }
      if (digitalRead(ENC_BTN) == LOW || digitalRead(BTN_START) == LOW) {
        delay(200); // Kurz entprellen, damit das Paddle nicht sofort springt
        break; // Bricht das Warten ab und die äußere Schleife startet von vorn!
      }
      delay(20);
    }
  } // Ende der while(playAgain) Schleife
}