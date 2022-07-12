/*  Header file for Raw_data structure, used for holding data
    interpreted by analyser thread for a single CPU to be computed
    and stored in Processed_data structure.

    Author: Szymon Przybysz */

#ifndef RAW_DATA_H
#define RAW_DATA_H

#include <stddef.h>

/* Number of fields in each row corresponding to CPU usage in /proc/stat */
#define N_FIELDS 10

/* Fields of each row corresponding to CPU usage in /proc/stat */
typedef enum Raw_data_field { user_fd, nice_fd, system_fd, idle_fd, iowait_fd, 
                              irq_fd, softirq_fd, steal_fd, guest_fd, guestnice_fd} Raw_data_field;

/* Forward declaration of Raw_data struct */

/* Holds data interpreted by analyser based on a signle line of /proc/stat - single CPU */
typedef struct Raw_data Raw_data;

/*  Constructor function for Raw_data struct 
    Returns pointer to Raw_data object on success and NULL otherwise */
Raw_data* raw_data_create(void);
/*  Destructor function for Raw_data_struct
    Returns without deallocating memory when rd is NULL */
void raw_data_destroy(Raw_data* rd);

/*  Sets value specified by val into field fd of Raw_data object pointed to by rd
    Returns -1 if rd is NULL, 0 otherwise */
int raw_data_set(Raw_data* rd, Raw_data_field fd, size_t val);
/*  Copies the first n_elems values from elems array (at most N_FIELDS elems are copied) into Raw_data object pointed to by rd
    Returns -1 if rd is NULL, -2 if elems is NULL, -3 if n_elems is equal 0, 0 otherwise */
int raw_data_fill(Raw_data* restrict rd, size_t const elems[restrict], size_t n_elems);
/*  Copies contents of Raw_data object pointed to by src_rd to object pointed to by dest_rd
    Returns -1 when dest_rd is NULL and -2 when src_rd is NULL, 0 otherwise */
int raw_data_copy(Raw_data* restrict dest_rd, Raw_data const* restrict src_rd);

/*  Calculates usage of a single CPU, based on data held by Raw_data objects pointed to by prev_rd and curr_rd 
    Returns -1 when prev_rd is NULL and -2 when curr_rd is NULL, returns calculated usage otherwise*/
double raw_data_calculate_usage(Raw_data const* prev_rd, Raw_data const* curr_rd);

#endif
