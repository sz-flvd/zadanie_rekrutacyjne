#include <printer.h>
#include <processed_data.h>
#include <message.h>
#include <warehouse.h>
#include <stdlib.h>
#include <unistd.h>

void* printer(void* arg) {
    Warehouse* w = *(Warehouse**) arg;
    
    while(true) {

        sleep(1);
    }
}
