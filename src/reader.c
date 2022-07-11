#include <warehouse.h>
#include <message.h>
#include <reader.h>
#include <thread_sleep.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define FILE_NAME "/proc/stat"
#define FILE_FLAG "r"
#define SLEEP_INFO_SIZE 34

void* reader(void* arg) {
    Warehouse* const w = *(Warehouse**)arg;
    srandom((unsigned)time(NULL));
    FILE* file;
    char* buf;
    int c;
    size_t buf_size;

    while(!warehouse_reader_is_done()) {
        file = fopen(FILE_NAME, FILE_FLAG);
        if(file == NULL) {
            warehouse_thread_put_to_logger(w, "[READER] File /proc/stat could not be opened", error);
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
            warehouse_thread_put_to_logger(w, "[READER] Determining size of buffer for Message failed", error);
            fclose(file);
            continue;
        }

        buf = calloc(1, buf_size + 1);
        if(buf == NULL) {
            warehouse_thread_put_to_logger(w, "[READER] Could not allocate memory for file buffer", error);
            fclose(file);
            continue;
        }

        if(fread(buf, buf_size, 1, file) != 1) {
            warehouse_thread_put_to_logger(w, "[READER] Reading file into buf failed", error);
            fclose(file);
            free(buf);
            continue;
        }
        
        fclose(file);

        Message* msg = message_create(raw_data, buf);
        /*
            At this point working with buf is done and it can be
            free'd whether or not message_create was successful
        */
        free(buf);
        if(msg == NULL) {
            warehouse_thread_put_to_logger(w, "[READER] Created NULL message, which may not be sent to warehouse", error);
            continue;
        }

        warehouse_thread_put_to_logger(w, "[READER] Entering critical section", info);
        warehouse_analyser_lock(w);
        if(warehouse_analyser_is_full(w)) {
            warehouse_thread_put_to_logger(w, "[READER] Queue is full, waiting for analyzer to finish processing and get next item", info);
            if(warehouse_reader_wait(w) != 0) {
                warehouse_analyser_unlock(w);
                message_destroy(msg);
                continue;
            }
        }

        warehouse_thread_put_to_logger(w, "[READER] Putting message in analyzer queue", info);
        warehouse_reader_put(w, msg);
        warehouse_analyser_get_notify(w);
        warehouse_thread_put_to_logger(w, "[READER] Leaving critical section", info);
        warehouse_analyser_unlock(w);

        warehouse_reader_notify_watchdog(w);

        long const sleep_dur = ((random() % 6) + 5) * 100;
        char info_buf[SLEEP_INFO_SIZE];
        sprintf(info_buf, "[READER] Sleeping for %ld millis", sleep_dur);
        warehouse_thread_put_to_logger(w, info_buf, info);
        thread_sleep_millis(sleep_dur);
    }

    return NULL;
}
