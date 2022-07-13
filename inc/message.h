/*  Header file for Message struct

    Author: Szymon Przybysz */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <stddef.h>
#include <time.h>

/*  Type of message, i.e. info, error etc. */
typedef enum Message_type {empty, raw_data, info, error, exit_info} Message_type;

/*  Forward declaration of Message struct */

/*  Holds a specified payload (char array), type and creation time */
typedef struct Message Message;

/*  Constructor function for Message struct
    Creates a message of given type, which contains payload comprised of chars from data
    Returns Message pointer on success or NULL otherwise */
Message* message_create(Message_type type, char const data[]);
/*  Destructor function for Message stuct
    Returns without deallocating memory if msg is NULL */
void message_destroy(Message* msg);

/*  Prints payload of Message pointed to by msg followed by newline character to standard output
    Returns without printing if msg is NULL */
void message_print(Message const* msg);

/*  Returns Message_type of Message pointed to by msg
    Returns empty if msg is NULL */
Message_type message_get_type(Message const* msg);
/*  Returns length of type string corresponding to Message_type of Message pointed to by msg
    (return value includes null terminating character)
    Return 0 if msg is NULL*/
size_t message_get_type_str_size(Message const* msg);
/*  Copies type string corresponding to Message_type of Message pointed to by msg to buf
    Returns 0 on success, returns -1 if msg is NULL, returns -2 if buf is NULL */
int message_get_type_str(Message const* restrict msg, char* restrict buf);
/*  Returns size of payload of Message pointed to by msg
    Returns 0 if msg is NULL */
size_t message_get_payload_size(Message const* msg);
/*  Copies payload of Message pointed to by msg to buf
    Returns 0 on success, returns -1 if msg is NULL, returns -2 if buf is NULL */
int message_get_payload(Message const* restrict msg, char* restrict buf);
/*  Copies creation time of Message pointed to by msg to dest_time
    Returns 0 on success, -1 if msg is NULL and -2 if dest_time is NULL */
int message_get_init_time(Message const* restrict msg, struct tm* restrict dest_time);

#endif
