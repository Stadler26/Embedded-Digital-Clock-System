#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <TM4C129.h>
#include "idle.h"

void Idle_Sleep(void) {
    __WFI();
}