#ifndef JK_06_01_COMMON_H
#define JK_06_01_COMMON_H

#define SERVER_QUEUE_PROJ_ID (225)

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
        } to_all;

        struct c2s_2one_msg_t
        {
            int client_id;
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
    } data;
} s2c_msg_t;

#endif
