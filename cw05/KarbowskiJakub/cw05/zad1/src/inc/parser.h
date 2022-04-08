#ifndef JK_05_01_PARSER_H
#define JK_05_01_PARSER_H

#include <stdbool.h>

#define PARSER_MAX_SYMBOL_SIZE (64)
#define PARSER_MAX_SYMBOLS (16)
#define PARSER_MAX_COMMAND_SIZE (128)
#define PARSER_MAX_COMMANDS (8)

typedef enum parser_state_t
{
    PARSER_S_INIT,
    PARSER_S_SYM1,
    PARSER_S_SYM1_WS,

    PARSER_S_PIPE_WS,
    PARSER_S_PIPE_SYM,
    PARSER_S_PIPE_SYM_WS,

    PARSER_S_ASSIGN_WS,
    PARSER_S_ASSIGN_CMD,
    PARSER_S_ASSIGN_CMD_PIPE_WS,

    PARSER_S_ERR,
} parser_state_t;

typedef struct parser_t
{
    parser_state_t state;
    const char *err_msg;

    char symbols[PARSER_MAX_SYMBOLS][PARSER_MAX_SYMBOL_SIZE + 1];
    int symbols_length[PARSER_MAX_SYMBOLS];
    int num_symbols;

    char commands[PARSER_MAX_COMMANDS][PARSER_MAX_COMMAND_SIZE + 1];
    int commands_length[PARSER_MAX_COMMANDS];
    int num_commands;

    bool comment_active;
} parser_t;

void parser_init(parser_t *parser);

int parser_feed(parser_t *parser, char c);

#endif
