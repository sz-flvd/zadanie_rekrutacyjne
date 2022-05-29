#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include <stdbool.h>

typedef enum Queue_status {queue_ok = 0, queue_empty = 1, queue_full = 2, queue_null = 3} Queue_status;

typedef struct Queue Queue;

Queue* queue_create(size_t max_size, size_t elem_size);
void queue_destroy(Queue* q);
bool queue_is_empty(Queue const* q);
bool queue_is_full(Queue const* q);
Queue_status queue_enqueue(Queue* q, void* elem);
void* queue_dequeue(Queue* q);

#endif
