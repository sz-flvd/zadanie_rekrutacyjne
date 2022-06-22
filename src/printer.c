#include <printer.h>
#include <processed_data.h>
#include <message.h>
#include <warehouse.h>
#include <stdlib.h>
#include <unistd.h>

void* printer(void* arg) {
    Warehouse* w = *(Warehouse**) arg;
    
    while(true) {
        printf("[PRINTER] Entering first critical section\n");
        warehouse_printer_lock(w);
        if(warehouse_printer_is_empty(w)) {
            printf("[PRINTER] Queue is empty, waiting for data from reader\n");
            warehouse_printer_wait(w);
        }
        printf("[PRINTER] Getting an object of Processed_data from queue\n");
        Processed_data** pd = warehouse_printer_get(w);
        warehouse_analyzer_put_notify(w);
        printf("[PRINTER] Leaving first critical section\n");
        warehouse_printer_unlock(w);

        /* Print contents of Processed_data object */
        for(size_t i = 0; i < processed_data_get_n_elem(pd); i++) {
            printf("CPU%d: %f\n", i, processed_data_get_elem_at(pd, i));
        }

        processed_data_destroy(*pd);
        free(pd);

        sleep(1);
    }
}
