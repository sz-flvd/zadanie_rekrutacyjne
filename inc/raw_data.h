#ifndef RAW_DATA_H
#define RAW_DATA_H

#include <stddef.h>

#define N_FIELDS 10

typedef enum Raw_data_field { user_fd, nice_fd, system_fd, idle_fd, iowait_fd, 
                              irq_fd, softirq_fd, steal_fd, guest_fd, guestnice_fd} Raw_data_field;

typedef struct Raw_data Raw_data;

Raw_data* raw_data_create();
void raw_data_destroy(Raw_data* rd);
int raw_data_set(Raw_data* rd, Raw_data_field fd, size_t elem);
int raw_data_fill(Raw_data* restrict rd, size_t const elems[restrict], size_t n_elems);
int raw_data_copy(Raw_data* restrict dest_rd, Raw_data* restrict src_rd);
double raw_data_calculate_usage(Raw_data const* prev_rd, Raw_data const* curr_rd);

#endif
