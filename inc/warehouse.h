#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <stdbool.h>

typedef struct Warehouse Warehouse;
typedef struct Warehouse_pair Warehouse_pair;

Warehouse* warehouse_create(void);
void warehouse_destroy(Warehouse* w);
bool warehouse_is_full(Warehouse const* w);
bool warehouse_is_empty(Warehouse const* w);
void warehouse_lock(Warehouse* w);
void warehouse_unlock(Warehouse* w);
void warehouse_put(Warehouse* w);
void* warehouse_get(Warehouse* w);
void warehouse_producer_wait(Warehouse* w);
void warehouse_consumer_wait(Warehouse* w);
void warehouse_producer_notify(Warehouse* w);
void warehouse_consumer_notify(Warehouse* w);

Warehouse_pair* warehouse_pair_create(Warehouse* w1, Warehouse* w2);
void Warehouse_pair_destroy(Warehouse_pair* wp);

#endif
