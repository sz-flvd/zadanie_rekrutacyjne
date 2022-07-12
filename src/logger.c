#include <logger.h>
#include <message.h>
#include <warehouse.h>
#include <thread_sleep.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#define LOG_FILE_NAME "log"
#define LOG_FILE_FLAG "a"
#define TIME_STR_SIZE 64
#define LOGGER_SLEEP_DUR 10
#define N_THREAD_EXIT 4
#define TOTAL_TIMEOUT 3

void* logger(void* arg) {
    Warehouse* w = *(Warehouse**)arg;
    FILE* file = fopen(LOG_FILE_NAME, LOG_FILE_FLAG);
    size_t exit_cnt = 0;
    size_t timeout_cnt = 0;
    bool done = false;
    
    while(!warehouse_logger_is_done() || !done) {
        warehouse_logger_lock(w);
        
        if(warehouse_logger_is_empty(w)) {
            warehouse_logger_unlock(w);
            if(warehouse_logger_full_pos_sem_wait(w) != 0) {
                timeout_cnt++;
                if(timeout_cnt >= TOTAL_TIMEOUT) {
                    done = true;
                }

                continue;
            } else timeout_cnt = 0;
            warehouse_logger_lock(w);
        } else {
            if(warehouse_logger_full_pos_sem_wait(w) != 0) {
                warehouse_logger_unlock(w);
                timeout_cnt++;
                if(timeout_cnt >= TOTAL_TIMEOUT) {
                    done = true;
                }

                continue;
            } else timeout_cnt = 0;
        }
        
        Message** msg = warehouse_logger_get(w);
        warehouse_logger_empty_pos_sem_post(w);
        warehouse_logger_unlock(w);

        if(msg == NULL) {
            continue;
        }

        char* msg_buf = malloc(message_get_payload_size(*msg));
        char* const type_buf = malloc(message_get_type_str_size(*msg));
        char time_str[TIME_STR_SIZE];
        struct tm* msg_init_time = malloc(sizeof(*msg_init_time));

        message_get_payload(*msg, msg_buf);
        message_get_init_time(*msg, msg_init_time);
        message_get_type_str(*msg, type_buf);
        strftime(time_str, sizeof(time_str), "%c", msg_init_time);
        
        fprintf(file, "#%s <%s> %s\n", time_str, type_buf, msg_buf);

        if(message_get_type(*msg) == exit_info) {
            exit_cnt++;
  
            if(exit_cnt == N_THREAD_EXIT) {
                done = true;
            }
        }

        free(msg_init_time);
        free(msg_buf);
        free(type_buf);
        message_destroy(*msg);
        free(msg);

        warehouse_logger_notify_watchdog(w);

        thread_sleep_millis(LOGGER_SLEEP_DUR);
    }

    fclose(file);
    
    return NULL;
}
