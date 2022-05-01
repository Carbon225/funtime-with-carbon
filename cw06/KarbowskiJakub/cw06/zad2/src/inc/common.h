#ifndef JK_06_01_COMMON_H
#define JK_06_01_COMMON_H

#include <time.h>

#define SERVER_QUEUE_PATH ("/jk_06_02_server_queue.0")
#define CLIENT_QUEUE_PATH_TEMPLATE ("/jk_06_02_client_queue.%ld")

#define MESSAGE_MAX_BODY_SIZE (256)

enum message_type_t
{
    MESSAGE_INIT,
    MESSAGE_2ONE,
    MESSAGE_2ALL,
    MESSAGE_LIST,
    MESSAGE_STOP,
};

// client to server message
typedef struct c2s_msg_t
{
    long type;
    union
    {
        struct c2s_init_msg_t
        {
            long client_queue_uid;
        } init;

        struct c2s_list_msg_t
        {
            int client_id;
        } list;

        struct c2s_2all_msg_t
        {
            int client_id;
            char body[MESSAGE_MAX_BODY_SIZE+1];
        } to_all;

        struct c2s_2one_msg_t
        {
            int client_id;
            int recipient_id;
            char body[MESSAGE_MAX_BODY_SIZE+1];
        } to_one;

        struct c2s_stop_msg_t
        {
            int client_id;
        } stop;
    } data;
} c2s_msg_t;

// server to client message
typedef struct s2c_msg_t
{
    long type;
    union
    {
        struct s2c_init_msg_t
        {
            int client_id;
        } init;

        struct s2c_list_msg_t
        {
            int client_id;
        } list;

        struct s2c_mail_msg_t
        {
            int sender_id;
            char body[MESSAGE_MAX_BODY_SIZE+1];
            time_t time;
        } mail;
    } data;
} s2c_msg_t;

#endif
