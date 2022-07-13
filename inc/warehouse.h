/*  Header file for Warehouse struct

    Author: Szymon Przybysz */

#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <message.h>
#include <processed_data.h>
#include <stdbool.h>

/*  Forward declaration of Warehouse struct */

/*  Main part of the project, Warehouse is responsible
    for communication and data transfer between threads */
typedef struct Warehouse Warehouse;

/*  Constructor function for Warehouse struct
    Return Warehouse pointer on success and NULL otherwise */
Warehouse* warehouse_create(void);
/*  Destructor function for Warehouse struct
    Returns without deallocating memory if w is NULL */
void warehouse_destroy(Warehouse* w);

/*  Returns value indicating if reader thread should end its work */
bool warehouse_reader_is_done(void);
/*  Returns value indicating if analyser thread should end its work */
bool warehouse_analyser_is_done(void);
/*  Returns value indicating if printer thread should end its work */
bool warehouse_printer_is_done(void);
/*  Returns value indicating if watchdog thread should end its work */
bool warehouse_watchdog_is_done(void);
/*  Returns value indicating if logger thread should end its work */
bool warehouse_logger_is_done(void);

/*  Returns value indicating if analyser queue is full
    Returns false if w is NULL */
bool warehouse_analyser_is_full(Warehouse const* w);
/*  Returns value indicating if analyser queue is empty
    Returns true if w is NULL */
bool warehouse_analyser_is_empty(Warehouse const* w);
/*  Returns value indicating if printer queue is full
    Returns false if w is NULL */
bool warehouse_printer_is_full(Warehouse const* w);
/*  Returns value indicating if printer queue is empty
    Returns true is w is NULL */
bool warehouse_printer_is_empty(Warehouse const* w);
/*  Returns value indicating if logger queue is full
    Returns false if w is NULL */
bool warehouse_logger_is_full(Warehouse const* w);
/*  Returns value indicating if logger queue is empty
    Returns true if w is NULL */
bool warehouse_logger_is_empty(Warehouse const* w);

/*  Locks mutex guarding analyser queue or returns without locking if w is NULL*/
void warehouse_analyser_lock(Warehouse* w);
/*  Unlocks mutex guarding analyser queue or returns without unlocking if w is NULL*/
void warehouse_analyser_unlock(Warehouse* w);
/*  Locks mutex guarding printer queue or returns without locking if w is NULL*/
void warehouse_printer_lock(Warehouse* w);
/*  Unlocks mutex guarding printer queue or returns without unlocking if w is NULL*/
void warehouse_printer_unlock(Warehouse* w);
/*  Locks mutex guarding logger queue or returns without locking if w is NULL*/
void warehouse_logger_lock(Warehouse* w);
/*  Unlocks mutex guarding logger queue or returns without unlocking if w is NULL*/
void warehouse_logger_unlock(Warehouse* w);

/*  Waits for at most 2 seconds on reader_put_allowed conditional variable
    Returns the value returned by pthread_cond_timedwait function or -1 if w is NULL*/
int warehouse_reader_wait(Warehouse* w);
/*  Signals reader_put_allowed conditional variable or returns without signalling if w is NULL */
void warehouse_reader_notify(Warehouse* w);
/*  Waits for at most 2 seconds on analyser_get_allowed conditional variable
    Returns the value returned by pthread_cond_timedwait function or -1 if w is NULL*/
int warehouse_analyser_get_wait(Warehouse* w);
/*  Signals analyser_get_allowed conditional variable or returns without signalling if w is NULL */
void warehouse_analyser_get_notify(Warehouse* w);
/*  Waits for at most 2 seconds on analyser_put_allowed conditional variable
    Returns the value returned by pthread_cond_timedwait function or -1 if w is NULL*/
int warehouse_analyser_put_wait(Warehouse* w);
/*  Signals analyser_put_allowed conditional variable or returns without signalling if w is NULL */
void warehouse_analyser_put_notify(Warehouse* w);
/*  Waits for at most 2 seconds on printer_get_allowed conditional variable
    Returns the value returned by pthread_cond_timedwait function or -1 if w is NULL*/
int warehouse_printer_wait(Warehouse* w);
/*  Signals printer_get_allowed conditional variable or returns without signalling if w is NULL */
void warehouse_printer_notify(Warehouse* w);
/*  Waits for at most 2 seconds on logger_empty_pos_sem semaphore
    Returns the value returned by sem_timedwait function or -1 if w is NULL*/
int warehouse_logger_empty_pos_sem_wait(Warehouse* w);
/*  Posts to logger_empty_pos_sem semaphore or returns without it if w is NULL */
void warehouse_logger_empty_pos_sem_post(Warehouse* w);
/*  Waits for at most 2 seconds on logger_full_pos_sem semaphore
    Returns the value returned by sem_timedwait function or -1 if w is NULL*/
int warehouse_logger_full_pos_sem_wait(Warehouse* w);
/*  Posts to logger_full_pos_sem semaphore or returns without it if w is NULL */
void warehouse_logger_full_pos_sem_post(Warehouse* w);

/*  Enqueues msg to analyser queue or returns without enqueueing if either w or msg is NULL */
void warehouse_reader_put(Warehouse* w, Message const* msg);
/*  Returns pointer to dequeued element of analyser queue or NULL on failure, including when w is NULL */
Message** warehouse_analyser_get(Warehouse* w);
/*  Enqueues pd to printer queue or returns without enqueueing if either w or pd is NULL */
void warehouse_analyser_put(Warehouse* w, Processed_data const* pd);
/*  Returns pointer to dequeued element of printer queue or NULL on failure, including when w is NULL */
Processed_data** warehouse_printer_get(Warehouse* w);
/*  Collective function used by all threads to put a Message specified by type and str to logger queue
    Enqueues a newly created Message struct pointer to logger queue or returns without enqueueing
    if either w or str is NULL, message creation is unsuccessful or timeout is exceeded*/
void warehouse_thread_put_to_logger(Warehouse* w, char const* str, Message_type type);
/*  Returns pointer to dequeued element of logger queue or NULL on failure, including when w is NULL */
Message** warehouse_logger_get(Warehouse* w);

/*  Sets reader_working value to 1 or returns if w is NULL */
void warehouse_reader_notify_watchdog(Warehouse* w);
/*  Sets analyser_working value to 1 or returns if w is NULL */
void warehouse_analyser_notify_watchdog(Warehouse* w);
/*  Sets printer_working value to 1 or returns if w is NULL */
void warehouse_printer_notify_watchdog(Warehouse* w);
/*  Sets logger_working value to 1 or returns if w is NULL */
void warehouse_logger_notify_watchdog(Warehouse* w);
/*  Fetches value of reader_working
    Returns -1 if w is NULL, 1 if watchdog was not notified or 0 otherwise*/
int warehouse_watchdog_check_reader(Warehouse* w);
/*  Fetches value of analyser_working
    Returns -1 if w is NULL, 1 if watchdog was not notified or 0 otherwise*/
int warehouse_watchdog_check_analyser(Warehouse* w);
/*  Fetches value of printer_working
    Returns -1 if w is NULL, 1 if watchdog was not notified or 0 otherwise*/
int warehouse_watchdog_check_printer(Warehouse* w);
/*  Fetches value of logger_working
    Returns -1 if w is NULL, 1 if watchdog was not notified or 0 otherwise*/
int warehouse_watchdog_check_logger(Warehouse* w);
/*  Calls terminate function, also used by signal handler */
void watchdog_terminate_threads(void);

#endif
