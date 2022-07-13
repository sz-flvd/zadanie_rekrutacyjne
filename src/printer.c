/*  Implementation of main printer thread function

    Author: Szymon Przybysz */

#include <printer.h>
#include <processed_data.h>
#include <message.h>
#include <warehouse.h>
#include <thread_sleep.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define PR_CRT_SEC_ENTER "[PRINTER] Entering critical section"
#define PR_QUEUE_EMPTY "[PRINTER] Queue is empty, waiting for data from analyzer"
#define PR_GET "[PRINTER] Getting an object of Processed_data from queue"
#define PR_CRT_SEC_EXIT "[PRINTER] Leaving critical section"
#define PR_NULL_PD "[PRINTER] Received NULL from queue"
#define PR_SLEEP "[PRINTER] Sleeping for 1 second"
#define PR_EXIT "[PRINTER] Exited main loop"

void* printer(void* arg) {
    Warehouse* w = *(Warehouse**)arg;
    
    while(!warehouse_printer_is_done()) {
        warehouse_thread_put_to_logger(w, PR_CRT_SEC_ENTER, info);
        warehouse_printer_lock(w);
        if(warehouse_printer_is_empty(w)) {
            warehouse_thread_put_to_logger(w, PR_QUEUE_EMPTY, info);
            if(warehouse_printer_wait(w) != 0) {
                warehouse_printer_unlock(w);
                continue;
            }
        }
        warehouse_thread_put_to_logger(w, PR_GET, info);
        Processed_data** pd = warehouse_printer_get(w);
        warehouse_analyser_put_notify(w);
        warehouse_thread_put_to_logger(w, PR_CRT_SEC_EXIT, info);
        warehouse_printer_unlock(w);

        if(pd == NULL) {
            warehouse_thread_put_to_logger(w, PR_NULL_PD, error);
            continue;
        }

        printf("\n---CPU USAGE----\n");
        for(size_t i = 0; i < processed_data_get_n_elem(*pd); i++) {
            printf("CPU%zu: %.5f[%%]\n", i, processed_data_get_elem_at(*pd, i));
        }
        printf("----------------\n");

        processed_data_destroy(*pd);
        free(pd);

        warehouse_printer_notify_watchdog(w);

        warehouse_thread_put_to_logger(w, PR_SLEEP, info);
        sleep(1);
    }

    warehouse_thread_put_to_logger(w, PR_EXIT, exit_info);

    return NULL;
}
