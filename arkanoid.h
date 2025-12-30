#ifndef ARKANOID_H
#define ARKANOID_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

void arkanoidGameInit();
void arkanoidGameUpdate();
bool arkanoidGameFinished();

#endif
