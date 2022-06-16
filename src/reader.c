#include <stdbool.h>
#include <stdio.h>
#include "warehouse.h"
#include "message.h"
#include "reader.h"

#define FILE_NAME "/proc/stat"
#define FILE_FLAG "r"

void* reader(void* arg) {
    Warehouse* w = *(Warehouse**) arg;
    FILE* file;
    char* const buf;
    size_t buf_size;

    while(true) {
        /* All error handling must include sending appropriate messages to logger */
        file = fopen(FILE_NAME, FILE_FLAG);
        if(file == NULL) {
            /* Change to handling errors with opening /proc/stat */
            printf("[READER] File %s could not be opened.\n", FILE_NAME);
            continue;
        }

        if(fseek(file, 0L, SEEK_END) != 0) {
            /* Change to handling errors with seeking file end */
            printf("[READER] Seeking file end in %s failed\n", FILE_NAME);
            fclose(file);
            continue;
        }
        buf_size = ftell(file);
        if(buf_size < 0) {
            /* Change to handling errors with seeking file end */
            printf("[READER] Error occured while determining file stream position\n",);
            fclose(file);
            continue;
        }
        rewind(file);

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
            At this point working with buf is done and it can be free'd 
            whether or not message_create was successful
        */
        free(buf);
        if(msg == NULL) {
            /* Change to sending the error message to logger also */
            printf("[READER] Created NULL message, which may not be sent to warehouse\n");
            continue;
        }

        warehouse_analyzer_lock(w);
        if(warehouse_analyzer_is_full(w)) {
            printf("[READER] Analyzer queue full, waiting to put msg\n");
            warehouse_reader_wait(w);
        }

        printf("[READER] Putting msg into analyzer queue\n");
        warehouse_analyzer_put(w, msg);
        warehouse_analyzer_get_notify(w);
        printf("[READER] Leaving critical section\n");
        warehouse_analyzer_unlock();

        message_destroy(msg);

        /* Perhaps sleep a random number of seconds/milliseconds, like rand() * 200 ms or something */
        sleep(1000);
    }
}
