/*  Header file for Queue structure

    Author: Szymon Przybysz */

#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include <stdbool.h>

/*  Possible status of queue, returned by queue_enqueue and queue_dequeue functions */
typedef enum Queue_status {queue_ok = 0, queue_empty = 1, queue_full = 2, queue_null = 3} Queue_status;

/*  Forward declaration of Queue structure */

/*  Generic queue struct, holds at most a specified number of elements, each of a specified size (same for every element) */
typedef struct Queue Queue;

/*  Constructor function for Queue struct
    Creates Queue with capacity of n_elem elements, each of which has size elem_size (in bytes)
    Returns pointer to Queue struct on successful creation, NULL otherwise */
Queue* queue_create(size_t n_elem, size_t elem_size);
/*  Destructor function for Queue struct
    Returns without deallocating memory when q is NULL */
void queue_destroy(Queue* q);

/*  Checks and returns if Queue pointed to by q is empty
    Returns true is q is NULL */
bool queue_is_empty(Queue const* q);
/*  Checks and returns if Queue pointed to by q is full
    Returns false if q is NULL */
bool queue_is_full(Queue const* q);
/*  Adds an element at the end of the Queue pointed to by q
    Returns appropriate Queue_status depending on whether Queue pointed to by q is NULL, full,
    if elem is NULL or if operation is succefull
    If no Queue_status indicating error is returned before, function copies bytes of elem at
    the end of the Queue pointed to by q*/
Queue_status queue_enqueue(Queue* restrict q, void const* restrict elem);
/*  Pops an element from the beginning of the Queue pointed to by q
    Returns appropriate Queue_status depending on whether Queue pointed to by q is NULL, empty,
    if elem is NULL or if operation is succesfull
    If no Queue_status indicating error is returned before, function copies bytes of the element
    at the beginning of Queue pointed to by q to elem */
Queue_status queue_dequeue(Queue* restrict q, void* restrict elem);

#endif
