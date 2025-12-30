#include "arkanoid.h"
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ======= OBJETOS COMPARTIDOS (desde main.ino) =======
extern Adafruit_SSD1306 display;
extern const int joystickXPin;
extern const int pausePin;

// ======= CONSTANTES =======
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// ======= ESTADO DEL JUEGO =======
static int paddleX;
static const int paddleY = 56;
static const int paddleWidth = 30;
static const int paddleHeight = 4;

static int ballX, ballY;
static int ballVX, ballVY;
static bool ballLaunched;

static const int brickRows = 3;
static const int brickCols = 8;
static const int brickWidth = 14;
static const int brickHeight = 6;

static bool bricks[brickRows][brickCols];

static int lives;
static bool gameOver;
static bool win;

// pausa
static bool paused;
static bool pauseLastState;

// =======================================
// FUNCIONES INTERNAS (NO visibles afuera)
// =======================================

static void resetBall() {
  ballX = paddleX + paddleWidth / 2;
  ballY = paddleY - 4;
  ballVX = 1;
  ballVY = -1;
  ballLaunched = false;
}

// ---------------------------------------
static void readPaddle() {
  int x = analogRead(joystickXPin);

  if (x < 400) paddleX -= 2;
  if (x > 600) paddleX += 2;

  if (paddleX < 0) paddleX = 0;
  if (paddleX > SCREEN_WIDTH - paddleWidth)
    paddleX = SCREEN_WIDTH - paddleWidth;
}

// ---------------------------------------
static void updateBall() {
  if (!ballLaunched) {
    ballX = paddleX + paddleWidth / 2;
    return;
  }

  ballX += ballVX;
  ballY += ballVY;

  // rebotes con bordes
  if (ballX <= 0 || ballX >= SCREEN_WIDTH - 2) ballVX *= -1;
  if (ballY <= 0) ballVY *= -1;

  // cae abajo
  if (ballY > SCREEN_HEIGHT) {
    lives--;
    resetBall();
    if (lives <= 0) gameOver = true;
  }

  // colisión con paddle
  if (ballY >= paddleY - 2 &&
      ballX >= paddleX &&
      ballX <= paddleX + paddleWidth) {
    ballVY *= -1;
  }

  // colisión con ladrillos
  bool bricksLeft = false;

  for (int r = 0; r < brickRows; r++) {
    for (int c = 0; c < brickCols; c++) {
      if (!bricks[r][c]) continue;

      bricksLeft = true;

      int bx = c * (brickWidth + 2) + 4;
      int by = r * (brickHeight + 2) + 4;

      if (ballX >= bx && ballX <= bx + brickWidth &&
          ballY >= by && ballY <= by + brickHeight) {
        bricks[r][c] = false;
        ballVY *= -1;
      }
    }
  }

  if (!bricksLeft) win = true;
}

// ---------------------------------------
static void drawGame() {
  display.clearDisplay();

  // paddle
  display.fillRect(paddleX, paddleY,
                   paddleWidth, paddleHeight, SSD1306_WHITE);

  // ball
  display.fillCircle(ballX, ballY, 2, SSD1306_WHITE);

  // bricks
  for (int r = 0; r < brickRows; r++) {
    for (int c = 0; c < brickCols; c++) {
      if (bricks[r][c]) {
        int bx = c * (brickWidth + 2) + 4;
        int by = r * (brickHeight + 2) + 4;
        display.fillRect(bx, by,
                         brickWidth, brickHeight, SSD1306_WHITE);
      }
    }
  }

  display.display();
}

// =======================================
// INTERFAZ PÚBLICA (la que usa el menú)
// =======================================

void arkanoidGameInit() {
  paddleX = 49;
  lives = 3;
  gameOver = false;
  win = false;
  paused = false;

  pauseLastState = HIGH;

  for (int r = 0; r < brickRows; r++)
    for (int c = 0; c < brickCols; c++)
      bricks[r][c] = true;

  resetBall();
}

// ---------------------------------------
void arkanoidGameUpdate() {
  bool pauseNow = digitalRead(pausePin);

  if (pauseLastState == HIGH && pauseNow == LOW) {
    paused = !paused;
  }
  pauseLastState = pauseNow;

  if (paused || gameOver || win) return;

  readPaddle();

  if (!ballLaunched && pauseNow == LOW) {
    ballLaunched = true;
  }

  updateBall();
  drawGame();
}

// ---------------------------------------
bool arkanoidGameFinished() {
  return gameOver || win;
}
