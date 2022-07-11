#include <warehouse.h>
#include <queue.h>
#include <processed_data.h>
#include <message.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <signal.h>

#define ANALYSER_QUEUE_N_ELEMS 5
#define PRINTER_QUEUE_N_ELEMS 5
#define LOGGER_QUEUE_N_ELEMS 5
#define WAIT_TIMEOUT_SEC 2

static sig_atomic_t volatile reader_done = 0;
static sig_atomic_t volatile analyser_done = 0;
static sig_atomic_t volatile printer_done = 0;
static sig_atomic_t volatile watchdog_done = 0;
static sig_atomic_t volatile logger_done = 0;

void terminate(int dummy);

void terminate(int const dummy) {
    reader_done = 1;
    analyser_done = 1;
    printer_done = 1;
    watchdog_done = 1;
    logger_done = 1;
}

struct timespec get_wait_time();

struct timespec get_wait_time() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += WAIT_TIMEOUT_SEC;
    return ts;
}


struct Warehouse {
    struct sigaction sigcatch;
    pthread_cond_t reader_put_allowed;
    pthread_cond_t analyser_get_allowed;
    pthread_cond_t analyser_put_allowed;
    pthread_cond_t printer_get_allowed;
    pthread_mutex_t analyser_mutex;
    pthread_mutex_t printer_mutex;
    pthread_mutex_t logger_mutex;
    pthread_mutex_t reader_working_mutex;
    pthread_mutex_t analyser_working_mutex;
    pthread_mutex_t printer_working_mutex;
    pthread_mutex_t logger_working_mutex;
    sem_t logger_empty_pos_sem;
    sem_t logger_full_pos_sem;
    Queue* analyser_queue;
    Queue* printer_queue;
    Queue* logger_queue;
    int reader_working;
    int analyser_working;
    int printer_working;
    int logger_working;
};

Warehouse* warehouse_create() {
    Warehouse* w = malloc(sizeof(*w));
    
    if(w == NULL) {
        return NULL;
    }

    *w = (Warehouse) {
        .sigcatch = {.sa_handler = terminate},
        .reader_put_allowed = PTHREAD_COND_INITIALIZER,
        .analyser_get_allowed = PTHREAD_COND_INITIALIZER,
        .analyser_put_allowed = PTHREAD_COND_INITIALIZER,
        .printer_get_allowed = PTHREAD_COND_INITIALIZER,
        .analyser_mutex = PTHREAD_MUTEX_INITIALIZER,
        .printer_mutex = PTHREAD_MUTEX_INITIALIZER,
        .logger_mutex = PTHREAD_MUTEX_INITIALIZER,
        .reader_working_mutex = PTHREAD_MUTEX_INITIALIZER,
        .analyser_working_mutex = PTHREAD_MUTEX_INITIALIZER,
        .printer_working_mutex = PTHREAD_MUTEX_INITIALIZER,
        .logger_working_mutex = PTHREAD_MUTEX_INITIALIZER,
        .analyser_queue = queue_create(ANALYSER_QUEUE_N_ELEMS, sizeof(Message*)),
        .printer_queue = queue_create(PRINTER_QUEUE_N_ELEMS, sizeof(Processed_data*)),
        .logger_queue = queue_create(LOGGER_QUEUE_N_ELEMS, sizeof(Message*)),
        .reader_working = 0,
        .analyser_working = 0,
        .printer_working = 0,
        .logger_working = 0
    };

    sigaction(SIGTERM, &w->sigcatch, NULL);

    sem_init(&w->logger_empty_pos_sem, 1, LOGGER_QUEUE_N_ELEMS);
    sem_init(&w->logger_full_pos_sem, 1, 0);

    return w;
}

void warehouse_destroy(Warehouse* const w) {
    if(w == NULL) {
        return;
    }

    while(!warehouse_analyser_is_empty(w)) {
        Message** const msg = warehouse_analyser_get(w);
        message_destroy(*msg);
        free(msg);
    }

    while(!warehouse_printer_is_empty(w)) {
        Processed_data** const pd = warehouse_printer_get(w);
        processed_data_destroy(*pd);
        free(pd);
    }

    while(!warehouse_logger_is_empty(w)) {
        Message** const msg = warehouse_logger_get(w);
        message_destroy(*msg);
        free(msg);
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
    pthread_mutex_destroy(&w->reader_working_mutex);
    pthread_mutex_destroy(&w->analyser_working_mutex);
    pthread_mutex_destroy(&w->printer_working_mutex);
    pthread_mutex_destroy(&w->logger_working_mutex);
    queue_destroy(w->analyser_queue);
    queue_destroy(w->printer_queue);
    queue_destroy(w->logger_queue);
    free(w);
}

bool warehouse_reader_is_done() {
    return reader_done != 0;
}

bool warehouse_analyser_is_done() {
    return analyser_done != 0;
}

bool warehouse_printer_is_done() {
    return printer_done != 0;
}

bool warehouse_watchdog_is_done() {
    return watchdog_done != 0;
}

bool warehouse_logger_is_done() {
    return logger_done != 0;
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

int warehouse_reader_wait(Warehouse* const w) {
    if(w == NULL) {
        return -1;
    }
    
    struct timespec wait_timeout = get_wait_time();
    return pthread_cond_timedwait(&w->reader_put_allowed, &w->analyser_mutex, &wait_timeout);
}

void warehouse_reader_notify(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_cond_signal(&w->reader_put_allowed);
}

int warehouse_analyser_get_wait(Warehouse* const w) {
    if(w == NULL) {
        return -1;
    }
    
    struct timespec wait_timeout = get_wait_time();
    return pthread_cond_timedwait(&w->analyser_get_allowed, &w->analyser_mutex, &wait_timeout);
}

void warehouse_analyser_get_notify(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_cond_signal(&w->analyser_get_allowed);
}

int warehouse_analyser_put_wait(Warehouse* const w) {
    if(w == NULL) {
        return -1;
    }
    
    struct timespec wait_timeout = get_wait_time();
    return pthread_cond_timedwait(&w->analyser_put_allowed, &w->printer_mutex, &wait_timeout);
}

void warehouse_analyser_put_notify(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_cond_signal(&w->analyser_put_allowed);
}

int warehouse_printer_wait(Warehouse* const w) {
    if(w == NULL) {
        return -1;
    }
    
    struct timespec wait_timeout = get_wait_time();
    return pthread_cond_timedwait(&w->printer_get_allowed, &w->printer_mutex, &wait_timeout);
}

void warehouse_printer_notify(Warehouse* const w) {
    if(w == NULL) {
        return;
    }
    
    pthread_cond_signal(&w->printer_get_allowed);
}

int warehouse_logger_empty_pos_sem_wait(Warehouse* w) {
    if(w == NULL) {
        return -1;
    }

    struct timespec wait_timeout = get_wait_time();
    return sem_timedwait(&w->logger_empty_pos_sem, &wait_timeout);
}

void warehouse_logger_empty_pos_sem_post(Warehouse* w) {
    if(w == NULL) {
        return;
    }

    sem_post(&w->logger_empty_pos_sem);
}

int warehouse_logger_full_pos_sem_wait(Warehouse* w) {
    if(w == NULL) {
        return -1;
    }

    struct timespec wait_timeout = get_wait_time();
    return sem_timedwait(&w->logger_full_pos_sem, &wait_timeout);
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

    queue_enqueue(w->analyser_queue, (void*) &msg);
}

Message** warehouse_analyser_get(Warehouse* const w) {
    if(w == NULL) {
        return NULL;
    }

    Message** const msg = malloc(sizeof(*msg));

    if(msg == NULL) {
        return NULL;
    }

    if(queue_dequeue(w->analyser_queue, (void*) msg) != queue_ok) {
        return NULL;
    }

    return msg;
}

void warehouse_analyser_put(Warehouse* const w, Processed_data const* const pd) {
    if(w == NULL || pd == NULL) {
        return;
    }

    queue_enqueue(w->printer_queue, (void*) &pd);
}

Processed_data** warehouse_printer_get(Warehouse* const w) {
    if(w == NULL) {
        return NULL;
    }

    Processed_data** pd = malloc(sizeof(*pd));

    if(pd == NULL) {
        return NULL;
    }

    if(queue_dequeue(w->printer_queue, (void*) pd) != queue_ok) {
        return NULL;
    }

    return pd;
}

void warehouse_thread_put_to_logger(Warehouse* const w, char const* const str, Message_type const type) {
    if(w == NULL || str == NULL) {
        return;
    }

    Message* msg = message_create(type, str);

    if(msg == NULL) {
        return;
    }

    warehouse_logger_lock(w);
  
    if(warehouse_logger_is_full(w)) {
        warehouse_logger_unlock(w);
        if(warehouse_logger_empty_pos_sem_wait(w) != 0) {
            message_destroy(msg);
            return;
        }
        warehouse_logger_lock(w);
    } else {
        if(warehouse_logger_empty_pos_sem_wait(w) != 0) {
            warehouse_logger_unlock(w);
            message_destroy(msg);
            return;
        }
    }
  
    queue_enqueue(w->logger_queue, (void*) &msg);
    warehouse_logger_full_pos_sem_post(w);
    warehouse_logger_unlock(w);
}

Message** warehouse_logger_get(Warehouse* const w) {
    if(w == NULL) {
        return NULL;
    }

    Message** msg = malloc(sizeof(*msg));

    if(msg == NULL) {
        return NULL;
    }

    if(queue_dequeue(w->logger_queue, (void*) msg) != queue_ok) {
        return NULL;
    }

    return msg;
}

void warehouse_reader_notify_watchdog(Warehouse* const w) {
    if(w == NULL) {
        return;
    }

    pthread_mutex_lock(&w->reader_working_mutex);
    w->reader_working = 1;
    pthread_mutex_unlock(&w->reader_working_mutex);
}

void warehouse_analyser_notify_watchdog(Warehouse* const w) {
    if(w == NULL) {
        return;
    }

    pthread_mutex_lock(&w->analyser_working_mutex);
    w->analyser_working = 1;
    pthread_mutex_unlock(&w->analyser_working_mutex);
}

void warehouse_printer_notify_watchdog(Warehouse* const w) {
    if(w == NULL) {
        return;
    }

    pthread_mutex_lock(&w->printer_working_mutex);
    w->printer_working = 1;
    pthread_mutex_unlock(&w->printer_working_mutex);
}

void warehouse_logger_notify_watchdog(Warehouse* const w) {
    if(w == NULL) {
        return;
    }

    pthread_mutex_lock(&w->logger_working_mutex);
    w->logger_working = 1;
    pthread_mutex_unlock(&w->logger_working_mutex);
}

int warehouse_watchdog_check_reader(Warehouse* const w) {
    if(w == NULL) {
        return -1;
    }

    int ret, curr;

    pthread_mutex_lock(&w->reader_working_mutex);
    curr = w->reader_working;
    ret = 1 - curr;             /* return value: 1 if watchdog was not notified, 0 otherwise */
    w->reader_working = 0;      /* end value should be either set to 0 after notification was received by watchdog or kept at 0 if watchdog was not notified */
    pthread_mutex_unlock(&w->reader_working_mutex);

    return ret;
}

int warehouse_watchdog_check_analyser(Warehouse* w) {
    if(w == NULL) {
        return -1;
    }

    int ret, curr;

    pthread_mutex_lock(&w->analyser_working_mutex);
    curr = w->analyser_working;
    ret = 1 - curr;
    w->analyser_working = 0;
    pthread_mutex_unlock(&w->analyser_working_mutex);

    return ret;
}

int warehouse_watchdog_check_printer(Warehouse* w) {
    if(w == NULL) {
        return -1;
    }

    int ret, curr;

    pthread_mutex_lock(&w->printer_working_mutex);
    curr = w->printer_working;
    ret = 1 - curr;
    w->printer_working = 0;
    pthread_mutex_unlock(&w->printer_working_mutex);

    return ret;
}

int warehouse_watchdog_check_logger(Warehouse* w) {
    if(w == NULL) {
        return -1;
    }

    int ret, curr;

    pthread_mutex_lock(&w->logger_working_mutex);
    curr = w->logger_working;
    ret = 1 - curr;
    w->logger_working = 0;
    pthread_mutex_unlock(&w->logger_working_mutex);

    return ret;
}

void watchdog_terminate_threads() {
    terminate(0);    
}
