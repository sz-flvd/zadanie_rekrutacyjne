#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <stdbool.h>

typedef struct Warehouse Warehouse;

Warehouse* warehouse_create(void);
void warehouse_destroy(Warehouse* w);

bool warehouse_analyzer_is_full(Warehouse const* w);
bool warehouse_analyzer_is_empty(Warehouse const* w);
bool warehouse_printer_is_full(Warehouse const* w);
bool warehouse_printer_is_empty(Warehouse const* w);
bool warehouse_logger_is_full(Warehouse const* w);
bool warehouse_logger_is_empty(Warehouse const* w);

void warehouse_analyzer_lock(Warehouse* w);
void warehouse_analyzer_unlock(Warehouse* w);
void warehouse_printer_lock(Warehouse* w);
void warehouse_printer_unlock(Warehouse* w);
void warehouse_logger_lock(Warehouse* w);
void warehouse_logger_unlock(Warehouse* w);

void warehouse_reader_put(Warehouse* w);
void* warehouse_analyzer_get(Warehouse* w);
void warehouse_analyzer_put(Warehouse* w);
void* warehouse_printer_get(Warehouse* w);
/* add function which allows tasks to post to logger queue */

void warehouse_reader_wait(Warehouse* w);
void warehouse_reader_notify(Warehouse* w);
void warehouse_analyzer_get_wait(Warehouse* w);
void warehouse_analyzer_get_notify(Warehouse* w);
void warehouse_analyzer_post_wait(Warehouse* w);
void warehouse_analyzer_post_notify(Warehouse* w);
void warehouse_printer_wait(Warehouse* w);
void warehouse_printer_notify(Warehouse* w);
/* add handling logger queue */

#endif
