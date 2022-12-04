#pragma once

// #define NAME "board-hex-leds"
#define NAME "board-hex-desk-leds"

#define USED_LED_CONFIG desk
// We have 500 in reality but don't use the last 10.
// To prevent stuck pixels we just act like they don't exist.
#define NUM_LEDS 490
// #define NUM_LEDS 275
