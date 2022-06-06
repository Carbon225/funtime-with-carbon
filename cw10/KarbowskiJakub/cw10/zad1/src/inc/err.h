#ifndef JK_10_01_ERR_H
#define JK_10_01_ERR_H

typedef enum err_t
{
    ERR_OK = 0,
    ERR_GENERIC = -1,
} err_t;

const char* err_msg(err_t err);

#endif
