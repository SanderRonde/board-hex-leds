#pragma once

// #define NAME "board-hex-leds"
#define NAME "board-hex-desk-leds"

#define USED_LED_CONFIG desk

#define NUM_LEDS_bed 275
// We have 500 in reality but don't use the last 10.
// To prevent stuck pixels we just act like they don't exist.
#define NUM_LEDS_desk 489
// I'm sure this can be done in a better way
#if USED_LED_CONFIG == desk
#define NUM_LEDS2 NUM_LEDS_desk
#elif USED_LED_CONFIG == bed
#define NUM_LEDS2 NUM_LEDS_bed
#endif