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

void* reader(void* arg) {
    Warehouse* const w = *(Warehouse**) arg;
    srandom(time(NULL));
    FILE* file;
    char* buf;
    char c;
    size_t buf_size;

    while(true) {
        /* All error handling must include sending appropriate messages to logger */
        file = fopen(FILE_NAME, FILE_FLAG);
        if(file == NULL) {
            /* Change to handling errors with opening /proc/stat */
            printf("[READER] File %s could not be opened\n", FILE_NAME);
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
            /* Change to handling errors with seeking file end */
            printf("[READER] Error occured while determining file stream position\n");
            fclose(file);
            continue;
        }

        buf = calloc(1, buf_size + 1);
        if(buf == NULL) {
            /* Change to handling errors with memory alloc */
            printf("[READER] Could not allocate %zu byte of memory for file buffer\n", buf_size);
            fclose(file);
            continue;
        }

        if(fread(buf, buf_size, 1, file) != 1) {
            /* Change to handling file read errors */
            printf("[READER] Reading file into buf failed\n");
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
            /* Change to sending the error message to logger also */
            printf("[READER] Created NULL message, which may not be sent to warehouse\n");
            continue;
        }

        printf("[READER] Entering critical section\n");
        warehouse_analyzer_lock(w);
        if(warehouse_analyzer_is_full(w)) {
            printf("[READER] Queue is full, waiting for analyzer to finish processing and get next item\n");
            warehouse_reader_wait(w);
        }

        printf("[READER] Putting message into analyzer queue\n");
        warehouse_reader_put(w, msg);
        warehouse_analyzer_get_notify(w);
        printf("[READER] Leaving critical section\n");
        warehouse_analyzer_unlock(w);

        long const rand_sleep = ((random() % 6) + 5) * 100;
        printf("[READER] Sleeping for %ld millis\n", rand_sleep);
        thread_sleep_millis(rand_sleep);
    }
}
