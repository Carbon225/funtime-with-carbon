#ifndef JK_06_01_COMMON_H

#define SERVER_QUEUE_PROJ_ID (225)

enum message_type_t
{
    MESSAGE_STOP,
    MESSAGE_LIST,
    MESSAGE_2ALL,
    MESSAGE_2ONE,
    MESSAGE_INIT,
};

typedef struct c2s_init_msg_t
{
    long type;
    struct
    {
        int client_queue;
    } data;
} c2s_init_msg_t;

typedef struct s2c_init_msg_t
{
    long client_id;
    struct
    {

    } data;
} s2c_init_msg_t;

typedef struct c2s_list_msg_t
{
    long type;
    struct
    {
        long client_id;
    } data;
} c2s_list_msg_t;

typedef struct c2s_2all_msg_t
{
    long type;
    struct
    {
        long client_id;
    } data;
} c2s_2all_msg_t;

typedef struct c2s_2one_msg_t
{
    long type;
    struct
    {
        long client_id;
    } data;
} c2s_2one_msg_t;

typedef struct c2s_stop_msg_t
{
    long type;
    struct
    {
        long client_id;
    } data;
} c2s_stop_msg_t;

#endif
