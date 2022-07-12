/*  Header file for Processed_data struct which holds calculated
    percentage usage for each CPU available.

    Author: Szymon Przybysz */

#ifndef PROCESSED_DATA_H
#define PROCESSED_DATA_H

#include <stddef.h>

/*  Forward delcaration of Processed_data struct */

/*  Holds usage of each CPU available (in percentage) */
typedef struct Processed_data Processed_data;

/*  Constructor function for Processed_data struct
    Returns Processed_data struct pointer on successful creation, NULL otherwise */
Processed_data* processed_data_create(size_t n_elem);
/*  Destructor function for Processed_data struct
    Returns without memory deallocation when pd is NULL */
void processed_data_destroy(Processed_data* pd);

/*  Sets a single value specified by val to object pointed to by pd at specified offset
    Returns -1 when pd is NULL, -2 if offset is greater than number of elements stored in pd, 0 otherwise */
int processed_data_set(Processed_data* pd, size_t offset, double val);
/*  Returns number of elements stored in object pointed to by pd or 0 if pd is NULL */
size_t processed_data_get_n_elem(Processed_data const* pd);
/*  Returns element stored in object pointed to by pd at given offset, return -1 if pd is NULL
    or -2 if offset is greater than total number of elements stored in object pointed to by pd */
double processed_data_get_elem_at(Processed_data const* pd, size_t offset);

#endif
