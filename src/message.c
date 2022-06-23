#include <message.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

struct Message {
    size_t payload_size;
    Message_type type;
    char payload[];
};

Message* message_create(Message_type const type, char const data[const]) {
    if(data == NULL) {
        return NULL;
    }

    const size_t len = strlen(data) + 1;

    Message* const message = malloc(sizeof(*message) + sizeof(*message->payload) * len);

    if(message == NULL) {
        return NULL;
    }

    message->payload_size = sizeof(*message->payload) * len;
    message->type = type;
    memcpy(message->payload, data, message->payload_size);

    return message;
}

void message_destroy(Message* const msg) {
    if(msg == NULL) {
        return;
    }
    
    free(msg);
}

void message_print(Message const* const msg) {
    if(msg == NULL) {
        return;
    }

    printf("%s\n", msg->payload);
}

size_t message_get_payload_size(Message const* const msg) {
    if(msg == NULL) {
        return 0;
    }

    return msg->payload_size;
}

int message_get_payload(Message const* const restrict msg, char* const restrict buf) {
    if(msg == NULL) {
        return -1;
    }

    if(buf == NULL) {
        return -2;
    }

    memcpy(buf, msg->payload, msg->payload_size);

    return 0;
}
