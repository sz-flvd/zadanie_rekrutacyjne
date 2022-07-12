#include <message.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

#define PADDING_SIZE 4

static char const* const msg_type_str[] = {"empty", "raw data", "info", "error", "exit_info"};

struct Message {
    struct tm init_time;
    size_t payload_size;
    Message_type type;
    char padding[PADDING_SIZE];
    char payload[];
};

Message* message_create(Message_type const type, char const data[const]) {
    if(data == NULL) {
        return NULL;
    }

    size_t const len = strlen(data) + 1;

    Message* const message = malloc(sizeof(*message) + sizeof(*message->payload) * len);

    if(message == NULL) {
        return NULL;
    }

    time_t const t = time(NULL);
    message->init_time = *localtime(&t);
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

Message_type message_get_type(Message const* const msg) {
    if(msg == NULL) {
        return empty;
    }

    return msg->type;
}

size_t message_get_type_str_size(Message const* const msg) {
    if(msg == NULL) {
        return 0;
    }

    return strlen(msg_type_str[msg->type]) + 1;
}

int message_get_type_str(Message const* const restrict msg, char* const restrict buf) {
    if(msg == NULL) {
        return -1;
    }

    if(buf == NULL) {
        return -2;
    }

    strcpy(buf, msg_type_str[msg->type]);

    return 0;
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

    strcpy(buf, msg->payload);

    return 0;
}

int message_get_init_time(Message const* const restrict msg, struct tm* const restrict dest_time) {
    if(msg == NULL) {
        return -1;
    }

    if(dest_time == NULL) {
        return -2;
    }

    memcpy(dest_time, &msg->init_time, sizeof(msg->init_time));
    
    return 0;
}
