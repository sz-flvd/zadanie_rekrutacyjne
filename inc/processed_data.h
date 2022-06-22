#ifndef PROCESSED_DATA_H
#define PROCESSED_DATA_H

#include <stddef.h>

typedef struct Processed_data Processed_data;

Processed_data* processed_data_create(size_t n_elem);
void processed_data_destroy(Processed_data* pd);
int processed_data_set(Processed_data* pd, size_t offset, double elem);
size_t processed_data_get_n_elem(Processed_data const* pd);
double processed_data_get_elem_at(Processed_data const* pd, size_t offset);

#endif
