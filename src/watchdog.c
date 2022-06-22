#include <watchdog.h>
#include <warehouse.h>
#include <stdbool.h>
#include <unistd.h>

void* watchdog(void* arg) {
    Warehouse* w = *(Warehouse**) arg;

    while(true) {
        
        sleep(1);
    }
}   
