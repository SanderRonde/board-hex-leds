#ifdef MOCK

#include "Arduino.a.h"

#include <math.h>
#include <sys/time.h>
#include <unistd.h>

unsigned long _millis(unsigned long subtract)
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	unsigned long ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
	return ms - subtract;
}
unsigned long time_subtract = 0;

// Various system functions
unsigned long millis() { return _millis(time_subtract); }

// Arduino functions
uint32_t analogRead(uint32_t pin) { return random(); }

void randomSeed(uint32_t dwSeed) { srand(dwSeed); }
long int random(int start, int end) { return (rand() * (end - start)) + start; }
void delay(unsigned long duration){};

#endif