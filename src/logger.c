#include <logger.h>
#include <message.h>
#include <warehouse.h>
#include <stdlib.h>
#include <unistd.h>


void* logger(void* arg) {
    Warehouse* w = *(Warehouse**) arg;
    while(true) {

        sleep(1);
    }
}
