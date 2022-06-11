#include "queue.h"

struct Queue {
    size_t n_elem;      /* maximum number of elements held in queue */
    size_t curr_elem;   /* current number of elements in queue */
    size_t elem_size;   /* size of single element in queues */
    /* add queue contents, */
};

Queue* queue_create(size_t n_elem, size_t elem_size) {
    if(n_elem == 0 || elem_size == 0) {
        return NULL;
    }

    Queue* q = malloc(sizeof(*q));

    if(q == NULL) {
        return NULL;
    }

    q->n_elem = n_elem;
    q->curr_elem = 0;
    q->elem_size = elem_size;
}

void queue_destroy(Queue* q) {
    if(q == NULL) {
        return NULL;
    }

    free(q);
}

bool queue_is_empty(Queue const* const q) {
    return (q->curr_elem == 0);
}

bool queue_is_full(Queue const* const q) {
    return (q->curr_elem == q->n_elem);
}

Queue_status queue_enqueue(Queue* const q, void* elem) {

}

void* queue_dequeue(Queue* const q) {

}
