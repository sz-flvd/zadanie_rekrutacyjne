#ifndef MESSAGE_H
#define MESSAGE_H

#include <stddef.h>
#include <time.h>

typedef enum Message_type {raw_data, debug, error} Message_type;

typedef struct Message Message;

Message* message_create(Message_type type, char const data[]);
void message_destroy(Message* msg);
void message_print(Message const* msg);
size_t message_get_payload_size(Message const* msg);
int message_get_payload(Message const* restrict msg, char* restrict buf);
int message_get_init_time(Message const* restrict msg, struct tm* restrict dest_time);

#endif
