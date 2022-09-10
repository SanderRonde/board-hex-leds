#ifdef MOCK

#include <unistd.h>
#include "Arduino.a.h"
#include "main.h"

int main(int argc, char *argv[])
{
    unsigned long setup_start = millis();
    setup();
    unsigned long setup_duration = millis() - setup_start;
    printf("[mock] setup done in %lu ms, running main loop\n",
           setup_duration);
    while (true)
    {
        loop();
    }

    return 0;
}

#endif