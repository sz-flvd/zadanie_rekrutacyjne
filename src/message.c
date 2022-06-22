#include <message.h>
#include <stdlib.h>
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
