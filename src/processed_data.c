#include <processed_data.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct Processed_data {
    size_t n_elem; /* Number of elements in FAM, equal to number of CPUs */
    double data[]; /* FAM containing CPU usage values [%] */
};

Processed_data* processed_data_create(size_t const n_elem) {
    Processed_data* pd = calloc(1, sizeof(*pd) + n_elem * sizeof(*pd->data));
    
    if(pd == NULL) {
        return NULL;
    }

    pd->n_elem = n_elem;

    return pd;
}

void processed_data_destroy(Processed_data* const pd) {
    if(pd == NULL) {
        return;
    }

    free(pd);
}

int processed_data_set(Processed_data* const pd, size_t const offset, double const val) {
    if(pd == NULL) {
        return -1;
    }

    if(offset >= pd->n_elem) {
        return -2;
    }

    pd->data[offset] = val;

    return 0;
}

size_t processed_data_get_n_elem(Processed_data const* const pd) {
    if(pd == NULL) {
        return 0;
    }

    return pd->n_elem;
}

double processed_data_get_elem_at(Processed_data const* const pd, size_t const offset) {
    if(pd == NULL) {
        return -1;
    }

    if(offset >= pd->n_elem) {
        return -2;
    }

    return pd->data[offset];
}
