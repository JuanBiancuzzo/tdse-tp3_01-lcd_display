//=====[Libraries]=============================================================

#include "mbed.h"
#include "smart_home_system.h"

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    smartHomeSystemInit();
    while (true) {
        smartHomeSystemUpdate();
    }

    printf("Hola tanto tiempo");
}