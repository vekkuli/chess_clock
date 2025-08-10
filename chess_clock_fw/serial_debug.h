#pragma once

#include <Arduino.h>

// #define DEBUG_SERIAL

#ifdef DEBUG_SERIAL
#define DEBUG_PRINTLN(x) Serial.println(x); Serial.flush()
#else
#define DEBUG_PRINTLN(x)
#endif