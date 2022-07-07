#include <watchdog.h>
#include <warehouse.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

void* watchdog(void* arg) {
    Warehouse* w = *(Warehouse**) arg;

    while(!warehouse_watchdog_is_done()) {
        
        sleep(1);
    }

    return NULL;
} 
