#include <warehouse.h>
#include <queue.h>
#include <processed_data.h>
#include <message.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define ANALYSER_QUEUE_N_ELEMS 5
#define PRINTER_QUEUE_N_ELEMS 5
#define LOGGER_QUEUE_N_ELEMS 5

struct Warehouse {
    Queue* analyser_queue;
    Queue* printer_queue;
    Queue* logger_queue;
    pthread_mutex_t analyser_mutex;
    pthread_mutex_t printer_mutex;
    pthread_mutex_t logger_mutex;
    pthread_cond_t reader_put_allowed;
    pthread_cond_t analyser_get_allowed;
    pthread_cond_t analyser_put_allowed;
    pthread_cond_t printer_get_allowed;
    sem_t logger_empty_pos_sem;
    sem_t logger_full_pos_sem;
};

Warehouse* warehouse_create() {
    Warehouse* w = malloc(sizeof(*w));

    if(w == NULL) {
        return NULL;
    }

    *w = (Warehouse) {
        .analyser_queue = queue_create(ANALYSER_QUEUE_N_ELEMS, sizeof(Message*)),
        .printer_queue = queue_create(PRINTER_QUEUE_N_ELEMS, sizeof(Processed_data*)),
        .logger_queue = queue_create(LOGGER_QUEUE_N_ELEMS, sizeof(Message*)),
        .analyser_mutex = PTHREAD_MUTEX_INITIALIZER,
        .printer_mutex = PTHREAD_MUTEX_INITIALIZER,
        .logger_mutex = PTHREAD_MUTEX_INITIALIZER,
        .reader_put_allowed = PTHREAD_COND_INITIALIZER,
        .analyser_get_allowed = PTHREAD_COND_INITIALIZER,
        .analyser_put_allowed = PTHREAD_COND_INITIALIZER,
        .printer_get_allowed = PTHREAD_COND_INITIALIZER
    };

    sem_init(&w->logger_empty_pos_sem, 1, LOGGER_QUEUE_N_ELEMS);
    sem_init(&w->logger_full_pos_sem, 1, 0);

    return w;
}

void warehouse_destroy(Warehouse* const w) {
    if(w == NULL) {
        return;
    }

    sem_destroy(&w->logger_empty_pos_sem);
    sem_destroy(&w->logger_full_pos_sem);
    pthread_cond_destroy(&w->reader_put_allowed);
    pthread_cond_destroy(&w->analyser_get_allowed);
    pthread_cond_destroy(&w->analyser_put_allowed);
    pthread_cond_destroy(&w->printer_get_allowed);
    pthread_mutex_destroy(&w->analyser_mutex);
    pthread_mutex_destroy(&w->printer_mutex);
    pthread_mutex_destroy(&w->logger_mutex);
    queue_destroy(w->analyser_queue);
    queue_destroy(w->printer_queue);
    queue_destroy(w->logger_queue);
    free(w);
}

bool warehouse_analyser_is_full(Warehouse const* const w) {
    if(w == NULL) {
        return false;
    }

    return queue_is_full(w->analyser_queue);
}

bool warehouse_analyser_is_empty(Warehouse const* const w) {
    if(w == NULL) {
        return true;
    }

    return queue_is_empty(w->analyser_queue);
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

void warehouse_analyser_lock(Warehouse* const w) {
    if(w == NULL) {
        return;
    }

    pthread_mutex_lock(&w->analyser_mutex);
}

void warehouse_analyser_unlock(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_mutex_unlock(&w->analyser_mutex);
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

void warehouse_logger_lock(Warehouse* w) {
    if(w == NULL) {
        return;
    }

    pthread_mutex_lock(&w->logger_mutex);
}

void warehouse_logger_unlock(Warehouse* w) {
    if(w == NULL) {
        return;
    }

    pthread_mutex_unlock(&w->logger_mutex);
}

void warehouse_reader_wait(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_cond_wait(&w->reader_put_allowed, &w->analyser_mutex);
}

void warehouse_reader_notify(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_cond_signal(&w->reader_put_allowed);
}

void warehouse_analyser_get_wait(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_cond_wait(&w->analyser_get_allowed, &w->analyser_mutex);
}

void warehouse_analyser_get_notify(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_cond_signal(&w->analyser_get_allowed);
}

void warehouse_analyser_put_wait(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_cond_wait(&w->analyser_put_allowed, &w->printer_mutex);
}

void warehouse_analyser_put_notify(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_cond_signal(&w->analyser_put_allowed);
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

void warehouse_logger_empty_pos_sem_wait(Warehouse* w) {
    if(w == NULL) {
        return;
    }

    sem_wait(&w->logger_empty_pos_sem);
}

void warehouse_logger_empty_pos_sem_post(Warehouse* w) {
    if(w == NULL) {
        return;
    }

    sem_post(&w->logger_empty_pos_sem);
}

void warehouse_logger_full_pos_sem_wait(Warehouse* w) {
    if(w == NULL) {
        return;
    }

    sem_wait(&w->logger_full_pos_sem);
}

void warehouse_logger_full_pos_sem_post(Warehouse* w) {
    if(w == NULL) {
        return;
    }

    sem_post(&w->logger_full_pos_sem);
}

void warehouse_reader_put(Warehouse* const w, Message const* const msg) {
    if(w == NULL || msg == NULL) {
        return; 
    }

    queue_enqueue(w->analyser_queue, &msg);
}

Message** warehouse_analyser_get(Warehouse* const w) {
    if(w == NULL) {
        return NULL;
    }

    Message** const msg = malloc(sizeof(*msg));

    if(msg == NULL) {
        return NULL;
    }

    if(queue_dequeue(w->analyser_queue, msg) != queue_ok) {
        return NULL;
    }

    return msg;
}

void warehouse_analyser_put(Warehouse* const w, Processed_data const* const pd) {
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

void warehouse_thread_put_to_logger(Warehouse* w, Message const* msg) {
    if(w == NULL || msg == NULL) {
        return;
    }

    queue_enqueue(w->logger_queue, &msg);
}

Message** warehouse_logger_get(Warehouse* w) {
    if(w == NULL) {
        return NULL;
    }

    Message** msg = malloc(sizeof(*w));

    if(msg == NULL) {
        return NULL;
    }

    if(queue_dequeue(w->logger_queue, msg) != queue_ok) {
        return NULL;
    }

    return msg;
}
