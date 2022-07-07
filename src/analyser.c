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

#define SLEEP_INFO_SIZE 36

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

    while(!warehouse_analyser_is_done()) {
        warehouse_thread_put_to_logger(w, "[ANALYSER] Entering first critical section", info);
        warehouse_analyser_lock(w);
        if(warehouse_analyser_is_empty(w)) {
            warehouse_thread_put_to_logger(w, "[ANALYSER] Queue is empty, waiting for data from reader", info);
            if(warehouse_analyser_get_wait(w) != 0) {
                warehouse_analyser_unlock(w);
                continue;
            }
        }
        warehouse_thread_put_to_logger(w, "[ANALYSER] Getting a message from queue", info);
        Message** msg = warehouse_analyser_get(w);
        warehouse_reader_notify(w);
        warehouse_thread_put_to_logger(w, "[ANALYSER] Leaving first critical section", info);
        warehouse_analyser_unlock(w);

        char* msg_buf = malloc(message_get_payload_size(*msg));
        
        if(msg_buf == NULL) {
            warehouse_thread_put_to_logger(w, "[ANALYSER] Couldn't allocate memory for Message buffer", error);
            message_destroy(*msg);
            free(msg);
            continue;
        }

        if(message_get_payload(*msg, msg_buf) != 0) {
            warehouse_thread_put_to_logger(w, "[ANALYSER] Getting Message payload failed", error);
            message_destroy(*msg);
            free(msg);
            continue;
        }

        message_destroy(*msg);
        free(msg);

        char* lines[n_procs + 1];
        char* save = msg_buf;
        char* token = strtok(save, "\n");
        size_t cnt = 0;

        while(cnt < n_procs + 1 && strncmp(token, "cpu", 3) == 0) {
            lines[cnt] = token;
            cnt++;
            token = strtok(NULL, "\n");
        }

        /* parse size_t elems from lines 1 to n_proc-1 (ignore line starting with cpu)*/
        for(size_t i = 1; i < cnt; i++) {
            save = lines[i];
            size_t fd_count = 0;
            token = strtok(save, " "); /* ignore the first column in line - cpu[n] */
            while((token = strtok(NULL, " ")) != NULL) {
                size_t elem = strtoumax(token, &(char*){NULL}, 10);
                raw_data_set(curr_rd[i - 1], (Raw_data_field) fd_count, elem);
                fd_count++;
            }
        }

        free(msg_buf);

        Processed_data* pd = processed_data_create(n_procs);

        if(pd == NULL) {
            warehouse_thread_put_to_logger(w, "[ANALYSER] Failed to create object of Processed_data", error);
            continue;
        }

        for(size_t i = 0; i < n_procs; i++) {
            double usg = raw_data_calculate_usage(prev_rd[i], curr_rd[i]);
            processed_data_set(pd, i, usg);
            raw_data_copy(prev_rd[i], curr_rd[i]);
        }

        warehouse_thread_put_to_logger(w, "[ANALYSER] Entering second critical section", info);
        warehouse_printer_lock(w);
        if(warehouse_printer_is_full(w)) {
            warehouse_thread_put_to_logger(w, "[ANALYSER] Queue is full, waiting for printer to finish printing processed data and get next item", info);
            if(warehouse_analyser_put_wait(w) != 0) {
                warehouse_printer_unlock(w);
                processed_data_destroy(pd);
                continue;
            }
        }
        warehouse_thread_put_to_logger(w, "[ANALYSER] Putting processed data object into queue", info);
        warehouse_analyser_put(w, pd);
        warehouse_printer_notify(w);
        warehouse_thread_put_to_logger(w, "[ANALYSER] Leaving second critical section", info);
        warehouse_printer_unlock(w);

        long const sleep_dur = ((random() % 6) + 5) * 100;
        char info_buf[SLEEP_INFO_SIZE];
        sprintf(info_buf, "[ANALYSER] Sleeping for %ld millis", sleep_dur);
        warehouse_thread_put_to_logger(w, info_buf, info);
        thread_sleep_millis(sleep_dur);
    }

    for(size_t i = 0; i < n_procs; i++) {
        raw_data_destroy(prev_rd[i]);
        raw_data_destroy(curr_rd[i]);
    }

    return NULL;
}
