#include <pthread.h>
#include "warehouse.h"
#include "queue.h"
#include "processed_data.h"
#include "message.h"

#define QUEUE_N_ELEMS 5

struct Warehouse {
    Queue* analyzer_queue;
    Queue* printer_queue;
    Queue* logger_queue;
    pthread_mutex_t analyzer_mutex;
    pthread_mutex_t printer_mutex;
    pthread_mutex_t logger_mutex;
    pthread_cond_t reader_post_allowed;
    pthread_cond_t analyzer_get_allowed;
    pthread_cond_t analyzer_post_allowed;
    pthread_cond_t printer_get_allowed;
    /* might need to change to semaphore */
    pthread_cond_t reader_log_allowed;
    pthread_cond_t analyzer_log_allowed;
    pthread_cond_t printer_log_allowed;
    pthread_cond_t watchdog_log_allowed;
};

Warehouse* warehouse_create(void) {
    Warehouse* w = malloc(sizeof(*w));

    if(w == NULL) {
        return NULL;
    }

    *w = (Warehouse) {
        .analyzer_queue = queue_create(QUEUE_N_ELEMS, sizeof(Message*)),
        .printer_queue = queue_create(QUEUE_N_ELEMS, sizeof(Processed_data*)),
        .logger_queue = queue_create(QUEUE_N_ELEMS, sizeof(Message*)),
        .analyzer_mutex = PTHREAD_MUTEX_INITIALIZER,
        .printer_mutex = PTHREAD_MUTEX_INITIALIZER,
        .logger_mutex = PTHREAD_MUTEX_INITIALIZER,
        .reader_post_allowed = PTHREAD_COND_INITIALIZER,
        .analyzer_get_allowed = PTHREAD_COND_INITIALIZER,
        .analyzer_post_allowed = PTHREAD_COND_INITIALIZER,
        .printer_get_allowed = PTHREAD_COND_INITIALIZER,
        .reader_log_allowed = PTHREAD_COND_INITIALIZER,
        .analyzer_log_allowed = PTHREAD_COND_INITIALIZER,
        .printer_log_allowed = PTHREAD_COND_INITIALIZER,
        .watchdog_log_allowed = PTHREAD_COND_INITIALIZER
    };

    return w;
}

void warehouse_destroy(Warehouse* w) {
    if(w == NULL) {
        return NULL;
    }

    pthread_cond_destroy(&w->reader_post_allowed);
    pthread_cond_destroy(&w->analyzer_get_allowed);
    pthread_cond_destroy(&w->analyzer_post_allowed);
    pthread_cond_destroy(&w->printer_get_allowed);
    pthread_cond_destroy(&w->reader_log_allowed);
    pthread_cond_destroy(&w->analyzer_log_allowed);
    pthread_cond_destroy(&w->printer_log_allowed);
    pthread_cond_destroy(&w->watchdog_log_allowed);
    pthread_mutex_destroy(&w->analyzer_mutex);
    pthread_mutex_destroy(&w->printer_mutex);
    pthread_mutex_destroy(&w->logger_mutex);
    queue_destroy(&w->analyzer_queue);
    queue_destroy(&w->printer_queue);
    queue_destroy(&w->logger_queue);
    free(w);
}

bool warehouse_analyzer_is_full(Warehouse const* w) {
    return queue_is_full(w->analyzer_queue);
}

bool warehouse_analyzer_is_empty(Warehouse const* w) {
    return queue_is_empty(w->analyzer_queue);
}

bool warehouse_printer_is_full(Warehouse const* w) {
    return queue_is_full(w->printer_queue);
}

bool warehouse_printer_is_empty(Warehouse const* w) {
    return queue_is_empty(w->printer_queue);
}

bool warehouse_logger_is_full(Warehouse const* w) {
    return queue_is_full(w->logger_queue);
}

bool warehouse_logger_is_empty(Warehouse const* w) {
    return queue_is_empty(w->logger_queue);
}

void warehouse_analyzer_lock(Warehouse* w) {
    
}

void warehouse_analyzer_unlock(Warehouse* w) {

}

void warehouse_printer_lock(Warehouse* w) {

}

void warehouse_printer_unlock(Warehouse* w) {

}

void warehouse_logger_lock(Warehouse* w) {

}

void warehouse_logger_unlock(Warehouse* w) {

}

void warehouse_reader_put(Warehouse* w) {

}

void* warehouse_analyzer_get(Warehouse* w) {

}

void warehouse_analyzer_put(Warehouse* w) {

}

void* warehouse_printer_get(Warehouse* w) {

}

/* add function which allows tasks to post to logger queue */

void warehouse_reader_wait(Warehouse* w) {

}

void warehouse_reader_notify(Warehouse* w) {

}

void warehouse_analyzer_get_wait(Warehouse* w) {

}

void warehouse_analyzer_get_notify(Warehouse* w) {

}

void warehouse_analyzer_post_wait(Warehouse* w) {

}

void warehouse_analyzer_post_notify(Warehouse* w) {

}

void warehouse_printer_wait(Warehouse* w) {

}

void warehouse_printer_notify(Warehouse* w) {
    
}

/* add handling logger queue */
