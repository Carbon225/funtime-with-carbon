#ifndef JK_06_01_COMMON_H
#define JK_06_01_COMMON_H

#define SERVER_QUEUE_PROJ_ID (225)

#define MESSAGE_MAX_BODY_SIZE (256)

enum message_type_t
{
    MESSAGE_STOP = 1,
    MESSAGE_LIST,
    MESSAGE_2ALL,
    MESSAGE_2ONE,
    MESSAGE_INIT,
};

// client to server message
typedef struct c2s_msg_t
{
    long type;
    union
    {
        struct c2s_init_msg_t
        {
            int client_queue;
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
        } mail;
    } data;
} s2c_msg_t;

#endif
