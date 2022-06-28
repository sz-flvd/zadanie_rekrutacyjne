#include <logger.h>
#include <message.h>
#include <warehouse.h>
#include <thread_sleep.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define LOG_FILE_NAME "log"
#define LOG_FILE_FLAG "a"
#define TIME_STR_SIZE 64
#define LOGGER_SLEEP_DUR 10

void* logger(void* arg) {
    Warehouse* w = *(Warehouse**) arg;
    //srandom(time(NULL));
    FILE* file = fopen(LOG_FILE_NAME, LOG_FILE_FLAG);

    if(file == NULL) {
        /* Do some error handling */
    }
    
    while(true) {
        warehouse_logger_lock(w);
        
        if(warehouse_logger_is_empty(w)) {
            warehouse_logger_unlock(w);
            warehouse_logger_full_pos_sem_wait(w);
            warehouse_logger_lock(w);
        } else {
            warehouse_logger_full_pos_sem_wait(w);
        }
        
        Message** msg = warehouse_logger_get(w);
        warehouse_logger_empty_pos_sem_post(w);
        warehouse_logger_unlock(w);

        if(msg == NULL) {
            continue;
        }

        char* msg_buf = malloc(message_get_payload_size(*msg));
        char* const type_buf = malloc(sizeof(message_get_type_str_size(*msg)));
        char time_str[TIME_STR_SIZE];
        struct tm* msg_init_time = malloc(sizeof(*msg_init_time));

        message_get_payload(*msg, msg_buf);
        message_get_init_time(*msg, msg_init_time);
        message_get_type_str(*msg, type_buf);
        strftime(time_str, sizeof(time_str), "%c", msg_init_time);
        
        fprintf(file, "#%s <%s> %s\n", time_str, type_buf, msg_buf);

        free(msg_init_time);
        free(msg_buf);
        free(type_buf);
        message_destroy(*msg);
        free(msg);

        thread_sleep_millis(LOGGER_SLEEP_DUR);
    }

    fclose(file);
}
