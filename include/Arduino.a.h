#pragma once

#ifndef MOCK
#include "Arduino.h"
#else
#pragma once

#include "stdlib.h"
#include <string>
#include "string.h"
#define String std::string
#define D5 5

#define boolean bool
#define Min(a, b) (((a) < (b)) ? (a) : (b))
#define Max(a, b) (((a) > (b)) ? (a) : (b))
#define sq(x) ((x)*(x))
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define radians(deg) ((deg)*DEG_TO_RAD)

typedef uint8_t byte;

uint32_t analogRead(uint32_t ulPin);

extern void randomSeed(uint32_t dwSeed);
unsigned long _millis();
unsigned long millis();

long int random(int min, int max);
void delay(unsigned long duration);

#define SERIAL_BUF_SIZE 64

class MockSerial {
   private:
    char serial_buf[SERIAL_BUF_SIZE] = {0};
    bool is_available = false;

    fd_set fds;

   public:
    void begin(const uint32_t dwBaudRate);
    size_t println(const char* x);
    size_t println(int x);
    size_t print(const char* x);
    size_t print(int x);
    size_t write(const char* chars);
		void flush();

    operator bool() { return true; };
};
extern MockSerial Serial;

#endif