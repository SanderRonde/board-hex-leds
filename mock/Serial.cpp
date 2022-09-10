#ifdef MOCK

#include "Arduino.a.h"

#include <math.h>
#include <sys/time.h>
#include <unistd.h>

#define SERIAL_BUF_SIZE 64

// Serial
int _read(int __fd, void *__buf, size_t __nbyte)
{
	return read(__fd, __buf, __nbyte);
}

void MockSerial::begin(const uint32_t dwBaudRate)
{
	printf("[mock] serial started, using baud rate %ul\n", dwBaudRate);
};
size_t MockSerial::println(const char *x) { return printf("%s\n", x); }
size_t MockSerial::println(int x) { return printf("%d\n", x); }
size_t MockSerial::print(const char *x) { return printf("%s", x); }
size_t MockSerial::print(int x) { return printf("%d", x); }
size_t MockSerial::write(const char *x) { return printf("%s", x); };
void MockSerial::flush() {};

MockSerial Serial;

#endif