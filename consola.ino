#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "snake.h"
#include "arkanoid.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ===== PINES =====
int joystickXPin = A0;
int joystickYPin = A1;
int joystickSW   = 7;
int pausePin = joystickSW;   // MISMO botón: menú + pausa


// ===== ESTADOS =====
enum AppState {
  STATE_MENU,
  STATE_GAME
};

enum GameType {
  GAME_SNAKE,
  GAME_ARKANOID
};

AppState currentState = STATE_MENU;
GameType currentGame  = GAME_SNAKE;

bool swLast = HIGH;
unsigned long lastMove = 0;

// --------------------------------
void drawMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(10, 4);
  display.print("MENU");

  display.setCursor(0, 20);
  display.print(currentGame == GAME_SNAKE ? "> Snake" : "  Snake");

  display.setCursor(0, 36);
  display.print(currentGame == GAME_ARKANOID ? "> Arkanoid" : "  Arkanoid");

  display.display();
}

// --------------------------------
void updateMenu() {
  int joyY = analogRead(joystickYPin);
  bool swNow = digitalRead(joystickSW);

  if (millis() - lastMove > 300) {
    if (joyY < 400) {
      currentGame = GAME_SNAKE;
      lastMove = millis();
    }
    else if (joyY > 600) {
      currentGame = GAME_ARKANOID;
      lastMove = millis();
    }
  }

  if (swLast == HIGH && swNow == LOW) {
      // Esperar a que suelte el switch antes de iniciar el juego
      while(digitalRead(joystickSW) == LOW) {
        delay(10); // espera corta, no bloquea demasiado
      }

      swLast = HIGH; // <--- actualizar el estado anterior

      currentState = STATE_GAME;

      if (currentGame == GAME_SNAKE) {
        snakeGameInit();
      } else {
        arkanoidGameInit();
      }
  }

  swLast = swNow;
  drawMenu();
}

// --------------------------------
void setup() {

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }
  display.setRotation(1); // rota 90° para vertical

  display.clearDisplay();
  display.display();

  pinMode(joystickSW, INPUT_PULLUP);
}

// --------------------------------
void loop() {

  if (currentState == STATE_MENU) {
    updateMenu();
  }
  else {

    if (currentGame == GAME_SNAKE) {
      snakeGameUpdate();
      if (snakeGameFinished()) {
        currentState = STATE_MENU;
      }
    }

    else if (currentGame == GAME_ARKANOID) {
      arkanoidGameUpdate();
      if (arkanoidGameFinished()) {
        currentState = STATE_MENU;
      }
    }

  }
}
