#include <warehouse.h>
#include <analyzer.h>
#include <message.h>
#include <raw_data.h>
#include <processed_data.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>

void* analyzer(void* arg) {
    Warehouse* w = *(Warehouse**) arg;
    int n_procs = get_nprocs();
    Raw_data* prev_rd[n_procs];
    Raw_data* curr_rd[n_procs];
    
    for(size_t i = 0; i < n_procs; i++) {
        prev_rd[i] = raw_data_create();
        curr_rd[i] = raw_data_create();
    }

    while(true) {
        printf("[ANALYZER] Entering first critical section\n");
        warehouse_analyzer_lock(w);
        if(warehouse_analyzer_is_empty(w)) {
            printf("[ANALYZER] Queue is empty, waiting for data from reader\n");
            warehouse_analyzer_get_wait(w);
        }
        printf("[ANALYZER] Getting a message from queue\n");
        Message** msg = warehouse_analyzer_get(w);
        warehouse_reader_notify(w);
        printf("[ANALYZER] Leaving first critical section\n");
        warehouse_analyzer_unlock(w);

        /* Do some analysis, create object of Processed_data */
        message_destroy(*msg);
        free(msg);

        Processed_data* pd = processed_data_create(n_procs);
        for(size_t i = 0; i < n_procs; i++) {
            size_t usg = raw_data_calculate_usage(prev_rd[i], curr_rd[i]);
            processed_data_set(pd, i, usg);
            raw_data_copy(prev_rd[i], curr_rd[i]);
        }

        printf("[ANALYZER] Entering second critical section\n");
        warehouse_printer_lock(w);
        if(warehouse_printer_is_full(w)) {
            printf("[ANALYZER] Queue is full, waiting for printer to finish printing processed data and get next item\n");
            warehouse_analyzer_put_wait(w);
        }
        printf("[ANALYZER] Putting processed data object into queue\n");
        warehouse_analyzer_put(w, pd);
        warehouse_printer_notify(w);
        printf("[ANALYZER] Leaving second critical section\n");

        /* Perhaps sleep a random number of seconds/milliseconds, like rand() * 200 ms or something */
        sleep(1);
    }
}
