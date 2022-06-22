#include <stdlib.h>
#include <pthread.h>
#include <warehouse.h>
#include <queue.h>
#include <processed_data.h>
#include <message.h>

#define QUEUE_N_ELEMS 5

struct Warehouse {
    Queue* analyzer_queue;
    Queue* printer_queue;
    Queue* logger_queue;
    pthread_mutex_t analyzer_mutex;
    pthread_mutex_t printer_mutex;
    pthread_mutex_t logger_mutex;
    pthread_cond_t reader_put_allowed;
    pthread_cond_t analyzer_get_allowed;
    pthread_cond_t analyzer_put_allowed;
    pthread_cond_t printer_get_allowed;
    /* might need to change to semaphore */
    pthread_cond_t reader_log_allowed;
    pthread_cond_t analyzer_log_allowed;
    pthread_cond_t printer_log_allowed;
    pthread_cond_t watchdog_log_allowed;
};

Warehouse* warehouse_create() {
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
        .reader_put_allowed = PTHREAD_COND_INITIALIZER,
        .analyzer_get_allowed = PTHREAD_COND_INITIALIZER,
        .analyzer_put_allowed = PTHREAD_COND_INITIALIZER,
        .printer_get_allowed = PTHREAD_COND_INITIALIZER,
        .reader_log_allowed = PTHREAD_COND_INITIALIZER,
        .analyzer_log_allowed = PTHREAD_COND_INITIALIZER,
        .printer_log_allowed = PTHREAD_COND_INITIALIZER,
        .watchdog_log_allowed = PTHREAD_COND_INITIALIZER
    };

    return w;
}

void warehouse_destroy(Warehouse* const w) {
    if(w == NULL) {
        return;
    }

    pthread_cond_destroy(&w->reader_put_allowed);
    pthread_cond_destroy(&w->analyzer_get_allowed);
    pthread_cond_destroy(&w->analyzer_put_allowed);
    pthread_cond_destroy(&w->printer_get_allowed);
    pthread_cond_destroy(&w->reader_log_allowed);
    pthread_cond_destroy(&w->analyzer_log_allowed);
    pthread_cond_destroy(&w->printer_log_allowed);
    pthread_cond_destroy(&w->watchdog_log_allowed);
    pthread_mutex_destroy(&w->analyzer_mutex);
    pthread_mutex_destroy(&w->printer_mutex);
    pthread_mutex_destroy(&w->logger_mutex);
    queue_destroy(w->analyzer_queue);
    queue_destroy(w->printer_queue);
    queue_destroy(w->logger_queue);
    free(w);
}

bool warehouse_analyzer_is_full(Warehouse const* const w) {
    if(w == NULL) {
        return false;
    }

    return queue_is_full(w->analyzer_queue);
}

bool warehouse_analyzer_is_empty(Warehouse const* const w) {
    if(w == NULL) {
        return true;
    }

    return queue_is_empty(w->analyzer_queue);
}

bool warehouse_printer_is_full(Warehouse const* const w) {
    if(w == NULL) {
        return false;
    }

    return queue_is_full(w->printer_queue);
}

bool warehouse_printer_is_empty(Warehouse const* const w) {
    if(w == NULL) {
        return true;
    }

    return queue_is_empty(w->printer_queue);
}

bool warehouse_logger_is_full(Warehouse const* const w) {
    if(w == NULL) {
        return false;
    }

    return queue_is_full(w->logger_queue);
}

bool warehouse_logger_is_empty(Warehouse const* const w) {
    if(w == NULL) {
        return true;
    }

    return queue_is_empty(w->logger_queue);
}

void warehouse_analyzer_lock(Warehouse* const w) {
    if(w == NULL) {
        return;
    }

    pthread_mutex_lock(&w->analyzer_mutex);
}

void warehouse_analyzer_unlock(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_mutex_unlock(&w->analyzer_mutex);
}

void warehouse_printer_lock(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_mutex_lock(&w->printer_mutex);
}

void warehouse_printer_unlock(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_mutex_unlock(&w->printer_mutex);
}

void warehouse_logger_lock(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
}

void warehouse_logger_unlock(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
}

void warehouse_reader_put(Warehouse* const w, Message const* const msg) {
    if(w == NULL || msg == NULL) {
        return; 
    }

    queue_enqueue(w->analyzer_queue, &msg);
}

Message** warehouse_analyzer_get(Warehouse* const w) {
    if(w == NULL) {
        return NULL;
    }

    Message** const msg = malloc(sizeof(*msg));

    if(msg == NULL) {
        return NULL;
    }

    if(queue_dequeue(w->analyzer_queue, msg) != queue_ok) {
        return NULL;
    }

    return msg;
}

void warehouse_analyzer_put(Warehouse* const w, Processed_data const* const pd) {
    if(w == NULL || pd == NULL) {
        return;
    }

    queue_enqueue(w->printer_queue, &pd);
}

Processed_data** warehouse_printer_get(Warehouse* const w) {
    if(w == NULL) {
        return NULL;
    }

    Processed_data** pd = malloc(sizeof(*pd));

    if(pd == NULL) {
        return NULL;
    }

    if(queue_dequeue(w->printer_queue, pd) != queue_ok) {
        return NULL;
    }

    return pd;
}

/* add function which allows tasks to post to logger queue */

void warehouse_reader_wait(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_cond_wait(&w->reader_put_allowed, &w->analyzer_mutex);
}

void warehouse_reader_notify(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_cond_signal(&w->reader_put_allowed);
}

void warehouse_analyzer_get_wait(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_cond_wait(&w->analyzer_get_allowed, &w->analyzer_mutex);
}

void warehouse_analyzer_get_notify(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_cond_signal(&w->analyzer_get_allowed);
}

void warehouse_analyzer_put_wait(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_cond_wait(&w->analyzer_put_allowed, &w->printer_mutex);
}

void warehouse_analyzer_put_notify(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_cond_signal(&w->analyzer_put_allowed);
}

void warehouse_printer_wait(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_cond_wait(&w->printer_get_allowed, &w->printer_mutex);
}

void warehouse_printer_notify(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_cond_signal(&w->printer_get_allowed);
}

/* add handling logger queue */
