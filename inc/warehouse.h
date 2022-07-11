#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <message.h>
#include <processed_data.h>
#include <stdbool.h>

typedef struct Warehouse Warehouse;

Warehouse* warehouse_create(void);
void warehouse_destroy(Warehouse* w);

bool warehouse_reader_is_done();
bool warehouse_analyser_is_done();
bool warehouse_printer_is_done();
bool warehouse_watchdog_is_done();
bool warehouse_logger_is_done();

bool warehouse_analyser_is_full(Warehouse const* w);
bool warehouse_analyser_is_empty(Warehouse const* w);
bool warehouse_printer_is_full(Warehouse const* w);
bool warehouse_printer_is_empty(Warehouse const* w);
bool warehouse_logger_is_full(Warehouse const* w);
bool warehouse_logger_is_empty(Warehouse const* w);

void warehouse_analyser_lock(Warehouse* w);
void warehouse_analyser_unlock(Warehouse* w);
void warehouse_printer_lock(Warehouse* w);
void warehouse_printer_unlock(Warehouse* w);
void warehouse_logger_lock(Warehouse* w);
void warehouse_logger_unlock(Warehouse* w);

int warehouse_reader_wait(Warehouse* w);
void warehouse_reader_notify(Warehouse* w);
int warehouse_analyser_get_wait(Warehouse* w);
void warehouse_analyser_get_notify(Warehouse* w);
int warehouse_analyser_put_wait(Warehouse* w);
void warehouse_analyser_put_notify(Warehouse* w);
int warehouse_printer_wait(Warehouse* w);
void warehouse_printer_notify(Warehouse* w);
int warehouse_logger_empty_pos_sem_wait(Warehouse* w);
void warehouse_logger_empty_pos_sem_post(Warehouse* w);
int warehouse_logger_full_pos_sem_wait(Warehouse* w);
void warehouse_logger_full_pos_sem_post(Warehouse* w);

void warehouse_reader_put(Warehouse* w, Message const* msg);
Message** warehouse_analyser_get(Warehouse* w);
void warehouse_analyser_put(Warehouse* w, Processed_data const* pd);
Processed_data** warehouse_printer_get(Warehouse* w);
void warehouse_thread_put_to_logger(Warehouse* w, char const* str, Message_type type);
Message** warehouse_logger_get(Warehouse* w);

void warehouse_reader_notify_watchdog(Warehouse* w);
void warehouse_analyser_notify_watchdog(Warehouse* w);
void warehouse_printer_notify_watchdog(Warehouse* w);
void warehouse_logger_notify_watchdog(Warehouse* w);
int warehouse_watchdog_check_reader(Warehouse* w);
int warehouse_watchdog_check_analyser(Warehouse* w);
int warehouse_watchdog_check_printer(Warehouse* w);
int warehouse_watchdog_check_logger(Warehouse* w);
void watchdog_terminate_threads();

#endif
