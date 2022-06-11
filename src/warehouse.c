#include <pthread.h>
#include "warehouse.h"
#include "queue.h"

struct Warehouse {
    Queue* q;
    pthread_mutex_t mutex;
    pthread_cond_t production_allowed;
    pthread_cond_t consumption_allowed;
};

Warehouse* warehouse_create(void) {
    Warehouse* w = malloc(sizeof(*w));

    if(w == NULL) {
        return NULL;
    }

    *w = (Warehouse) {
        .q = queue_create,
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .production_allowed = PTHREAD_COND_INITIALIZER,
        .consumption_allowed = PTHREAD_COND_INITIALIZER
    };

    return w;
}

void warehouse_destroy(Warehouse* w) {
    if(w == NULL) {
        return NULL;
    }

    pthread_cond_destroy(&w->consumption_allowed);
    pthread_cond_destroy(&w->production_allowed);
    pthread_mutex_destroy(&w->mutex);
    queue_destroy(&w->q);
    free(w);
}

bool warehouse_is_full(Warehouse const* w) {

}

bool warehouse_is_empty(Warehouse const* w) {

}

void warehouse_lock(Warehouse* w) {

}

void warehouse_unlock(Warehouse* w) {

}

void warehouse_put(Warehouse* w) {

}

void* warehouse_get(Warehouse* w) {

}

void warehouse_producer_wait(Warehouse* w) {

}

void warehouse_consumer_wait(Warehouse* w) {
    
}

void warehouse_producer_notify(Warehouse* w) {

}

void warehouse_consumer_notify(Warehouse* w) {
    
}
