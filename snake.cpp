#include "snake.h"

// ===== REFERENCIAS COMPARTIDAS =====
extern Adafruit_SSD1306 display;
extern int joystickXPin;
extern int joystickYPin;

// ===== CONFIG =====
static const int cellSize   = 4;
static const int gridWidth  = 32;
static const int gridHeight = 16;

// ===== ESTADO =====
static int snakeX, snakeY;
static int foodX, foodY;
static int snakeSize;
static char direction;
static int tailX[100], tailY[100];
static bool snakeOver;

static unsigned long lastMove;
static const unsigned long snakeSpeed = 250;

// --------------------------------
static void spawnFood() {
  foodX = random(gridWidth);
  foodY = random(gridHeight);
}

// --------------------------------
void snakeGameInit() {
  snakeX = 4;
  snakeY = 4;
  snakeSize = 1;
  direction = ' ';
  snakeOver = false;
  lastMove = millis();
  spawnFood();

  display.clearDisplay();
  display.display();
}

// --------------------------------
static void readJoystick() {
  int x = analogRead(joystickXPin);
  int y = analogRead(joystickYPin);

  // EJE Y NORMAL (arriba = subir)
  if (y < 200) direction = 'u';
  else if (y > 800) direction = 'd';

  // EJE X NORMAL
  else if (x < 200) direction = 'l';
  else if (x > 800) direction = 'r';
}

// --------------------------------
static void moveSnake() {
  for (int i = snakeSize - 1; i > 0; i--) {
    tailX[i] = tailX[i - 1];
    tailY[i] = tailY[i - 1];
  }

  tailX[0] = snakeX;
  tailY[0] = snakeY;

  if (direction == 'l') snakeX--;
  if (direction == 'r') snakeX++;
  if (direction == 'u') snakeY--;
  if (direction == 'd') snakeY++;

  if (snakeX < 0) snakeX = gridWidth - 1;
  if (snakeX >= gridWidth) snakeX = 0;
  if (snakeY < 0) snakeY = gridHeight - 1;
  if (snakeY >= gridHeight) snakeY = 0;

  if (snakeX == foodX && snakeY == foodY) {
    snakeSize++;
    spawnFood();
    if (snakeSize > 20) snakeOver = true;
  }
}

// --------------------------------
static void drawSnake() {
  display.clearDisplay();

  display.fillRect(
    foodX * cellSize,
    foodY * cellSize,
    cellSize,
    cellSize,
    SSD1306_WHITE
  );

  for (int i = 0; i < snakeSize; i++) {
    display.fillRect(
      tailX[i] * cellSize,
      tailY[i] * cellSize,
      cellSize,
      cellSize,
      SSD1306_WHITE
    );
  }

  display.display();
}

// --------------------------------
void snakeGameUpdate() {
  if (snakeOver) return;

  readJoystick();

  if (millis() - lastMove > snakeSpeed) {
    lastMove = millis();
    moveSnake();
    drawSnake();
  }
}

// --------------------------------
bool snakeGameFinished() {
  return snakeOver;
}
