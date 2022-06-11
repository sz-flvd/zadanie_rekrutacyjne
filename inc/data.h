#ifndef DATA_H
#define DATA_H

typedef struct raw_data raw_data;
typedef struct processed_data processed_data;

raw_data* raw_data_create(char const content[]);

processed_data* processed_data_create(double const content[]);

#endif
