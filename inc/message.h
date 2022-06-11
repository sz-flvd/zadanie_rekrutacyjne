#ifndef MESSAGE_H
#define MESSAGE_H

typedef enum Message_type {raw_data, debug, error} Message_type;

typedef struct Message Message;

Message* message_create(Message_type type, char const data[]);
void message_destroy(Message* msg);

#endif
