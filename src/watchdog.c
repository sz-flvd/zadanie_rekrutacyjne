#include <stdbool.h>
#include "watchdog.h"

void* watchdog(void* arg) {
    while(true) {
        sleep(100);
    }
}   
