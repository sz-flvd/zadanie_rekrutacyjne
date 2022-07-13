/*  Implementation of main reader thread function

    Author: Szymon Przybysz */

#include <warehouse.h>
#include <message.h>
#include <reader.h>
#include <thread_sleep.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#define RD_OPEN_ERR "[READER] File /proc/stat could not be opened"
#define RD_MSG_BUF_SIZE_ERR "[READER] Determining size of buffer for Message failed"
#define RD_MSG_BUF_ALLOC_ERR "[READER] Could not allocate memory for file buffer"
#define RD_READ_ERR "[READER] Reading file into buf failed"
#define RD_NULL_MSG_ERR "[READER] Created NULL message, which may not be sent to warehouse"
#define RD_CRT_SEC_ENTER "[READER] Entering critical section"
#define RD_QUEUE_FULL "[READER] Queue is full, waiting for analyzer to finish processing and get next item"
#define RD_PUT "[READER] Putting message in analyzer queue"
#define RD_CRT_SEC_EXIT "[READER] Leaving critical section"
#define RD_SLEEP "[READER] Sleeping for 1 second"
#define RD_EXIT "[READER] Exited main loop"

#define FILE_NAME "/proc/stat"
#define FILE_FLAG "r"

void* reader(void* arg) {
    Warehouse* const w = *(Warehouse**)arg;
    FILE* file;
    char* buf;
    int c;
    size_t buf_size;

    while(!warehouse_reader_is_done()) {
        file = fopen(FILE_NAME, FILE_FLAG);
        if(file == NULL) {
            warehouse_thread_put_to_logger(w, RD_OPEN_ERR, error);
            continue;
        }

        buf_size = 0;
        
        do {
            c = fgetc(file);
            buf_size++;
        } while (c != EOF);
            
        buf_size -= 1;
        rewind(file);

        if(buf_size == 0) {
            warehouse_thread_put_to_logger(w, RD_MSG_BUF_SIZE_ERR, error);
            fclose(file);
            continue;
        }

        buf = calloc(1, buf_size + 1);
        if(buf == NULL) {
            warehouse_thread_put_to_logger(w, RD_MSG_BUF_ALLOC_ERR, error);
            fclose(file);
            continue;
        }

        if(fread(buf, buf_size, 1, file) != 1) {
            warehouse_thread_put_to_logger(w, RD_READ_ERR, error);
            fclose(file);
            free(buf);
            continue;
        }
        
        fclose(file);

        Message* msg = message_create(raw_data, buf);
        /*  At this point working with buf is done and it can be
            free'd whether or not message_create was successful */
        free(buf);
        if(msg == NULL) {
            warehouse_thread_put_to_logger(w, RD_NULL_MSG_ERR, error);
            continue;
        }

        warehouse_thread_put_to_logger(w, RD_CRT_SEC_ENTER, info);
        warehouse_analyser_lock(w);
        if(warehouse_analyser_is_full(w)) {
            warehouse_thread_put_to_logger(w, RD_QUEUE_FULL, info);
            if(warehouse_reader_wait(w) != 0) {
                warehouse_analyser_unlock(w);
                message_destroy(msg);
                continue;
            }
        }

        warehouse_thread_put_to_logger(w, RD_PUT, info);
        warehouse_reader_put(w, msg);
        warehouse_analyser_get_notify(w);
        warehouse_thread_put_to_logger(w, RD_CRT_SEC_EXIT, info);
        warehouse_analyser_unlock(w);

        warehouse_reader_notify_watchdog(w);

        warehouse_thread_put_to_logger(w, RD_SLEEP, info);
        sleep(1);
    }

    warehouse_thread_put_to_logger(w, RD_EXIT, exit_info);

    return NULL;
}
