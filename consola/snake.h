#ifndef SNAKE_H
#define SNAKE_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// interfaz común para el menú
void snakeGameInit();
void snakeGameUpdate();
bool snakeGameFinished();

#endif
