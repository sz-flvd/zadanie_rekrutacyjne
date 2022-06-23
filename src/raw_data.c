#include <raw_data.h>
#include <stdlib.h>
#include <string.h>

struct Raw_data {
    size_t fields[N_FIELDS];
};

Raw_data* raw_data_create() {
    Raw_data* rd = calloc(1, sizeof(*rd));

    if(rd == NULL) {
        return NULL;
    }

    return rd;
}

void raw_data_destroy(Raw_data* const rd) {
    if(rd == NULL) {
        return;
    }

    free(rd);
}

int raw_data_set(Raw_data* const rd, Raw_data_field const fd, size_t const elem) {
    if(rd == NULL) {
        return -1;
    }

    rd->fields[fd] = elem;

    return 0;
}

int raw_data_fill(Raw_data* const restrict rd, size_t const elems[const restrict static 1], size_t const n_elems) {
    if(rd == NULL) {
        return -1;
    }

    if(elems == NULL) {
        return -2;
    }

    if(n_elems == 0) {
        return -3;
    }

    size_t len = n_elems < N_FIELDS ? n_elems : N_FIELDS;
    memcpy(rd->fields, elems, len * sizeof(*elems));

    return 0;
}

int raw_data_copy(Raw_data* const restrict dest_rd, Raw_data* const restrict src_rd) {
    if(dest_rd == NULL) {
        return -1;
    }

    if(src_rd == NULL) {
        return -2;
    }

    raw_data_fill(dest_rd, src_rd->fields, N_FIELDS);

    return 0;
}

double raw_data_calculate_usage(Raw_data const* const prev_rd, Raw_data const* const curr_rd) {
    size_t prev_idle = prev_rd->fields[idle_fd] + prev_rd->fields[iowait_fd];
    size_t curr_idle = curr_rd->fields[idle_fd] + curr_rd->fields[iowait_fd];

    size_t prev_non_idle = prev_rd->fields[user_fd] + prev_rd->fields[nice_fd] + prev_rd->fields[system_fd] + prev_rd->fields[irq_fd] + prev_rd->fields[softirq_fd] + prev_rd->fields[steal_fd];
    size_t curr_non_idle = curr_rd->fields[user_fd] + curr_rd->fields[nice_fd] + curr_rd->fields[system_fd] + curr_rd->fields[irq_fd] + curr_rd->fields[softirq_fd] + curr_rd->fields[steal_fd];

    size_t prev_total = prev_idle + prev_non_idle;
    size_t curr_total = curr_idle + curr_non_idle;

    size_t total_d = curr_total - prev_total;
    size_t idle_d = curr_idle - prev_idle;

    double usage = (double)(total_d - idle_d) / total_d;

    return usage;
}
