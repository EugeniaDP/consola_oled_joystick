#include "snake.h"
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// ===== REFERENCIAS COMPARTIDAS =====
extern Adafruit_SSD1306 display;
extern int joystickXPin;
extern int joystickYPin;
extern int pausePin;   // Pin del switch para pausa

// ===== CONFIG =====
static const int cellSize   = 4;
static const int gridWidth  = 16;
static const int gridHeight = 32;

// ===== ESTADO =====
static int snakeX, snakeY;
static int foodX, foodY;
static int snakeSize;
static char direction;
static int tailX[100], tailY[100];
static bool snakeOver;

static unsigned long lastMove;
static const unsigned long snakeSpeed = 200;

// ===== PAUSA =====
static bool paused = false;
static bool pauseLastState = HIGH;

// --------------------------------
static void spawnFood() {
    foodX = random(gridWidth);
    foodY = random(gridHeight);
}

// --------------------------------
void snakeGameInit() {
    snakeX = 8;
    snakeY = 16;
    snakeSize = 1;
    direction = ' ';
    snakeOver = false;
    lastMove = millis();
    spawnFood();

    // Inicializar cola para evitar falsa colisión
    for (int i = 0; i < 100; i++) {
        tailX[i] = snakeX;
        tailY[i] = snakeY;
    }

    paused = false;
    pauseLastState = HIGH;   // inicializamos igual que Arkanoid

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
    // Mover cola
    for (int i = snakeSize - 1; i > 0; i--) {
        tailX[i] = tailX[i - 1];
        tailY[i] = tailY[i - 1];
    }
    tailX[0] = snakeX;
    tailY[0] = snakeY;

    // Actualizar cabeza
    if (direction == 'l') snakeX--;
    if (direction == 'r') snakeX++;
    if (direction == 'u') snakeY--;
    if (direction == 'd') snakeY++;

    // Colisión con bordes
    if (snakeX < 0 || snakeX >= gridWidth || snakeY < 0 || snakeY >= gridHeight) {
        snakeOver = true;
        return;
    }

    // Colisión con la propia serpiente (se inicia en 1 porque, si se inicia en 0, la condición es verdadera al iniciar el juego)
    for (int i = 1; i < snakeSize; i++) {
        if (snakeX == tailX[i] && snakeY == tailY[i]) {
            snakeOver = true;
            return;
        }
    }

    // Comer comida
    if (snakeX == foodX && snakeY == foodY) {
        snakeSize++;
        spawnFood();
        if (snakeSize > 100) snakeOver = true; // límite máximo de tamaño
    }
}

// --------------------------------
static void drawSnake() {
    display.clearDisplay();

    // Dibujar comida
    display.fillRect(
        foodX * cellSize,
        foodY * cellSize,
        cellSize,
        cellSize,
        SSD1306_WHITE
    );

    // Dibujar serpiente
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
static void checkPause() {
    bool pauseNow = digitalRead(pausePin);

    if (pauseLastState == HIGH && pauseNow == LOW) {
        paused = !paused;
    }

    pauseLastState = pauseNow;
}

// --------------------------------
void snakeGameUpdate() {
    if (snakeOver) return;

    checkPause();  // revisar pausa

    if (!paused) {
        readJoystick();

        if (millis() - lastMove > snakeSpeed) {
            lastMove = millis();
            moveSnake();
            drawSnake();
        }
    }
}

// --------------------------------
bool snakeGameFinished() {
    return snakeOver;
}
