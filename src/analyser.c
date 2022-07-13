/*  Implementation of main analyser thread function

    Author: Szymon Przybysz */

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
#include <inttypes.h>
#include <unistd.h>
#include <sys/sysinfo.h>

#define AN_CRT_SEC_1_ENTER "[ANALYSER] Entering first critical section"
#define AN_QUEUE_EMPTY "[ANALYSER] Queue is empty, waiting for data from reader"
#define AN_GET "[ANALYSER] Getting a message from queue"
#define AN_CRT_SEC_1_EXIT "[ANALYSER] Leaving first critical section"
#define AN_NULL_MSG "[ANALYSER] Received NULL from queue"
#define AN_BUF_ALLOC_ERR "[ANALYSER] Couldn't allocate memory for Message buffer"
#define AN_MSG_PAYLOAD_ERR "[ANALYSER] Getting Message payload failed"
#define AN_PD_CREATE_ERR "[ANALYSER] Failed to create object of Processed_data"
#define AN_CRT_SEC_2_ENTER "[ANALYSER] Entering second critical section"
#define AN_QUEUE_FULL "[ANALYSER] Queue is full, waiting for printer to finish printing processed data and get next item"
#define AN_PUT "[ANALYSER] Putting processed data object into queue"
#define AN_CRT_SEC_2_EXIT "[ANALYSER] Leaving second critical section"
#define AN_SLEEP "[ANALYSER] Sleeping for 1 second"
#define AN_EXIT "[ANALYSER] Exited main loop"

void* analyser(void* arg) {
    Warehouse* w = *(Warehouse**)arg;
    size_t n_procs = (size_t) get_nprocs();
    Raw_data* prev_rd[n_procs];
    Raw_data* curr_rd[n_procs];
    
    for(size_t i = 0; i < n_procs; i++) {
        prev_rd[i] = raw_data_create();
        curr_rd[i] = raw_data_create();
    }

    while(!warehouse_analyser_is_done()) {
        warehouse_thread_put_to_logger(w, AN_CRT_SEC_1_ENTER, info);
        warehouse_analyser_lock(w);
        if(warehouse_analyser_is_empty(w)) {
            warehouse_thread_put_to_logger(w, AN_QUEUE_EMPTY, info);
            if(warehouse_analyser_get_wait(w) != 0) {
                warehouse_analyser_unlock(w);
                continue;
            }
        }
        warehouse_thread_put_to_logger(w, AN_GET, info);
        Message** msg = warehouse_analyser_get(w);
        warehouse_reader_notify(w);
        warehouse_thread_put_to_logger(w, AN_CRT_SEC_1_EXIT, info);
        warehouse_analyser_unlock(w);

        if(msg == NULL) {
            warehouse_thread_put_to_logger(w, AN_NULL_MSG, error);
            continue;
        }

        char* msg_buf = malloc(message_get_payload_size(*msg));
        
        if(msg_buf == NULL) {
            warehouse_thread_put_to_logger(w, AN_BUF_ALLOC_ERR, error);
            message_destroy(*msg);
            free(msg);
            continue;
        }

        if(message_get_payload(*msg, msg_buf) != 0) {
            warehouse_thread_put_to_logger(w, AN_MSG_PAYLOAD_ERR, error);
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
            warehouse_thread_put_to_logger(w, AN_PD_CREATE_ERR, error);
            continue;
        }

        for(size_t i = 0; i < n_procs; i++) {
            double usg = raw_data_calculate_usage(prev_rd[i], curr_rd[i]);
            processed_data_set(pd, i, usg);
            raw_data_copy(prev_rd[i], curr_rd[i]);
        }

        warehouse_thread_put_to_logger(w, AN_CRT_SEC_2_ENTER, info);
        warehouse_printer_lock(w);
        if(warehouse_printer_is_full(w)) {
            warehouse_thread_put_to_logger(w, AN_QUEUE_FULL, info);
            if(warehouse_analyser_put_wait(w) != 0) {
                warehouse_printer_unlock(w);
                processed_data_destroy(pd);
                continue;
            }
        }
        warehouse_thread_put_to_logger(w, AN_PUT, info);
        warehouse_analyser_put(w, pd);
        warehouse_printer_notify(w);
        warehouse_thread_put_to_logger(w, AN_CRT_SEC_2_EXIT, info);
        warehouse_printer_unlock(w);

        warehouse_analyser_notify_watchdog(w);

        warehouse_thread_put_to_logger(w, AN_SLEEP, info);
        sleep(1);
    }

    warehouse_thread_put_to_logger(w, AN_EXIT, exit_info);

    for(size_t i = 0; i < n_procs; i++) {
        raw_data_destroy(prev_rd[i]);
        raw_data_destroy(curr_rd[i]);
    }

    return NULL;
}
