#include <warehouse.h>
#include <analyser.h>
#include <message.h>
#include <raw_data.h>
#include <processed_data.h>
#include <thread_sleep.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <time.h>
#include <sys/sysinfo.h>

/*  TODO
    - add error handling (malloc, all possible NULLs etc.)
        * all errors should be handled and reported to logger eventually
*/

void* analyser(void* arg) {
    Warehouse* w = *(Warehouse**) arg;
    srandom(time(NULL));
    size_t n_procs = (size_t) get_nprocs();
    Raw_data* prev_rd[n_procs];
    Raw_data* curr_rd[n_procs];
    
    for(size_t i = 0; i < n_procs; i++) {
        prev_rd[i] = raw_data_create();
        curr_rd[i] = raw_data_create();
    }

    while(true) {
        printf("[ANALYSER] Entering first critical section\n");
        warehouse_analyser_lock(w);
        if(warehouse_analyser_is_empty(w)) {
            printf("[ANALYSER] Queue is empty, waiting for data from reader\n");
            warehouse_analyser_get_wait(w);
        }
        printf("[ANALYSER] Getting a message from queue\n");
        Message** msg = warehouse_analyser_get(w);
        warehouse_reader_notify(w);
        printf("[ANALYSER] Leaving first critical section\n");
        warehouse_analyser_unlock(w);

        char* msg_buf = malloc(message_get_payload_size(*msg));
        message_get_payload(*msg, msg_buf);
        char* lines[n_procs + 1];
        char* token;
        char* save = msg_buf;
        size_t cnt = 0;

        while(cnt < n_procs + 1 && strncmp((token = strtok_r(save, "\n", &save)), "cpu", 3) == 0) {
            lines[cnt] = token;
            cnt++;
        }

        free(msg_buf);

        /* parse size_t elems from lines 1 to n_proc-1 (ignore line starting with cpu)*/
        for(size_t i = 1; i < cnt; i++) {
            save = lines[i];
            size_t fd_count = 0;
            token = strtok_r(save, " ", &save); /* ignore the first column in line - cpu[n] */
            while((token = strtok_r(NULL, " ", &save)) != NULL) {
                size_t elem = strtoumax(token, &(char*){NULL}, 10);
                raw_data_set(curr_rd[i - 1], (Raw_data_field) fd_count, elem);
                fd_count++;
            }
        }

        message_destroy(*msg);
        free(msg);

        Processed_data* pd = processed_data_create(n_procs);
        for(size_t i = 0; i < n_procs; i++) {
            double usg = raw_data_calculate_usage(prev_rd[i], curr_rd[i]);
            processed_data_set(pd, i, usg);
            raw_data_copy(prev_rd[i], curr_rd[i]);
        }

        printf("[ANALYSER] Entering second critical section\n");
        warehouse_printer_lock(w);
        if(warehouse_printer_is_full(w)) {
            printf("[ANALYSER] Queue is full, waiting for printer to finish printing processed data and get next item\n");
            warehouse_analyser_put_wait(w);
        }
        printf("[ANALYSER] Putting processed data object into queue\n");
        warehouse_analyser_put(w, pd);
        warehouse_printer_notify(w);
        printf("[ANALYSER] Leaving second critical section\n");
        warehouse_printer_unlock(w);

        long const sleep_dur = ((random() % 6) + 5) * 100;
        printf("[ANALYSER] Sleeping for %ld millis\n", sleep_dur);
        thread_sleep_millis(sleep_dur);
    }
}
