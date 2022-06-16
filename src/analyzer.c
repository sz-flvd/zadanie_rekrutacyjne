#include <stdbool.h>
#include "warehouse.h"
#include "analyzer.h"
#include "message.h"
#include "processed_data.h"

void* analyzer(void* arg) {
    Warehouse* w = *(Warehouse**) arg;

    while(true) {

        sleep(1000);
    }
}
