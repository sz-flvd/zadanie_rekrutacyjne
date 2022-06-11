#include <stdlib.h>
#include "processed_data.h"

struct Processed_data {
    
};

Processed_data* processed_data_create() {
    Processed_data* data = malloc(sizeof(*data));

    if(data == NULL) {
        return NULL;
    }

    return data;
}
