#include <printer.h>
#include <processed_data.h>
#include <message.h>
#include <warehouse.h>
#include <thread_sleep.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void* printer(void* arg) {
    Warehouse* w = *(Warehouse**) arg;
    
    while(!warehouse_printer_is_done()) {
        warehouse_thread_put_to_logger(w, "[PRINTER] Entering critical section", info);
        warehouse_printer_lock(w);
        if(warehouse_printer_is_empty(w)) {
            warehouse_thread_put_to_logger(w, "[PRINTER] Queue is empty, waiting for data from analyzer", info);
            if(warehouse_printer_wait(w) != 0) {
                warehouse_printer_unlock(w);
                continue;
            }
        }
        warehouse_thread_put_to_logger(w, "[PRINTER] Getting an object of Processed_data from queue", info);
        Processed_data** pd = warehouse_printer_get(w);
        warehouse_analyser_put_notify(w);
        warehouse_thread_put_to_logger(w, "[PRINTER] Leaving critical section", info);
        warehouse_printer_unlock(w);

        for(size_t i = 0; i < processed_data_get_n_elem(*pd); i++) {
            printf("CPU%zu: %.5f[%%]\n", i, processed_data_get_elem_at(*pd, i));
        }

        processed_data_destroy(*pd);
        free(pd);

        warehouse_thread_put_to_logger(w, "[PRINTER] Sleeping for 1 second", info);
        sleep(1);
    }

    return NULL;
}
