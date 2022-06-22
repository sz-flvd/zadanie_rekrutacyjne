#include <queue.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct Queue {
    size_t n_elem;      /* maximum number of elements held in queue */
    size_t curr_n_elem; /* current number of elements in queue */
    size_t elem_size;   /* size of single element in queues */
    size_t head;
    size_t tail;
    uint8_t content[];  /* FAM */
 };

Queue* queue_create(size_t const n_elem, size_t const elem_size) {
    if(n_elem == 0 || elem_size == 0) {
        return NULL;
    }

    Queue* q = calloc(1, sizeof(*q) + n_elem * elem_size);

    if(q == NULL) {
        return NULL;
    }

    q->n_elem = n_elem;
    q->elem_size = elem_size;

    return q;
}

void queue_destroy(Queue* const q) {
    if(q == NULL) {
        return;
    }

    free(q);
}

bool queue_is_empty(Queue const* const q) {
    if(q == NULL) {
        return true;
    }

    return (q->curr_n_elem == 0);
}

bool queue_is_full(Queue const* const q) {
    if(q == NULL) {
        return false;
    }

    return (q->curr_n_elem == q->n_elem);
}

Queue_status queue_enqueue(Queue* const restrict q, void const* const restrict elem) {
    if(q == NULL || elem == NULL) {
        return queue_null;
    }

    if(queue_is_full(q)) {
        return queue_full;
    }

    uint8_t*const ptr = &q->content[q->head * q->elem_size];
    memcpy(ptr, elem, q->elem_size);

    q->head++;
    if(q->head >= q->n_elem) {
        q->head = 0;
    }

    q->curr_n_elem++;

    return queue_ok;
}

Queue_status queue_dequeue(Queue* const restrict q, void* const restrict elem) {
    if(q == NULL || elem == NULL) {
        return queue_null;
    }

    if(queue_is_empty(q)) {
        return queue_empty;
    }

    const uint8_t* const ptr = &q->content[q->tail * q->elem_size];
    memcpy(elem, ptr, q->elem_size);

    q->tail++;
    if(q->tail >= q->n_elem) {
        q->tail = 0;
    }

    q->curr_n_elem--;

    return queue_ok;
}
