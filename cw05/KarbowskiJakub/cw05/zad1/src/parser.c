#include "parser.h"

#include <stdio.h>
#include <string.h>
#include "executor.h"

#define CHAR_CLASS(class, chars)                                   \
    static const char CHAR_CLASS_##class[] = chars;                \
    static bool char_is_##class(char c)                            \
    {                                                              \
        for (int i = 0; i < sizeof(CHAR_CLASS_##class) - 1; ++i)   \
            if (CHAR_CLASS_##class[i] == c) return true;           \
        return false;                                              \
    }

CHAR_CLASS(symbol,
           "qwertyuiopasdfghjklzxcvbnm"
           "QWERTYUIOPASDFGHJKLZXCVBNM"
           "ęóąśłżźćń"
           "ęóąśłżźćń"
           "1234567890"
           "_-"
           )

CHAR_CLASS(whitespace,
           " \t\r"
           )

CHAR_CLASS(newline,
           "\n"
           )

CHAR_CLASS(pipe,
           "|"
           )

CHAR_CLASS(assign,
           "="
           )

static bool char_is_command(char c)
{
    return !char_is_newline(c) &&
           !char_is_pipe(c);
}

static void parser_push_symbol(parser_t *parser, char c)
{
    int i = parser->num_symbols - 1;

    if (parser->symbols_length[i] >= PARSER_MAX_SYMBOL_SIZE)
    {
        parser->state = PARSER_S_ERR;
        parser->err_msg = "Symbol exceeded maximum allowed length\n";
    }
    else
    {
        parser->symbols[i][parser->symbols_length[i]] = c;
        parser->symbols_length[i]++;
        parser->symbols[i][parser->symbols_length[i]] = 0;
    }
}

static void parser_new_symbol(parser_t *parser, char c)
{
    if (parser->num_symbols >= PARSER_MAX_SYMBOLS)
    {
        parser->state = PARSER_S_ERR;
        parser->err_msg = "Exceeded maximum number of symbols in execute expression";
    }
    else
    {
        parser->symbols[parser->num_symbols][0] = c;
        parser->symbols[parser->num_symbols][1] = 0;
        parser->symbols_length[parser->num_symbols] = 1;
        parser->num_symbols++;
    }
}

static void parser_push_command(parser_t *parser, char c)
{
    int i = parser->num_commands - 1;

    if (parser->commands_length[i] >= PARSER_MAX_COMMAND_SIZE)
    {
        parser->state = PARSER_S_ERR;
        parser->err_msg = "Command exceeded maximum allowed length\n";
    }
    else
    {
        parser->commands[i][parser->commands_length[i]] = c;
        parser->commands_length[i]++;
        parser->commands[i][parser->commands_length[i]] = 0;
    }
}

static void parser_new_command(parser_t *parser, char c)
{
    if (parser->num_commands >= PARSER_MAX_COMMANDS)
    {
        parser->state = PARSER_S_ERR;
        parser->err_msg = "Exceeded maximum number of commands in assign expression";
    }
    else
    {
        parser->commands[parser->num_commands][0] = c;
        parser->commands[parser->num_commands][1] = 0;
        parser->commands_length[parser->num_commands] = 1;
        parser->num_commands++;
    }
}

static void parser_execute(parser_t *parser)
{
    program_t *p = &parser->program;
    exec_expr_t expr;

    for (int i = 0; i < parser->num_symbols; ++i)
    {
        int matches = 0;
        for (int j = 0; j < p->num_assign_exprs; ++j)
        {
            if (!strcmp(parser->symbols[i], p->assign_exprs[j].symbol))
            {
                expr.symbols[i] = j;
                matches++;
            }
        }
        if (matches < 1)
        {
            parser->state = PARSER_S_ERR;
            parser->err_msg = "Undefined symbol in execute expression";
            return;
        }
        else if (matches > 1)
        {
            parser->state = PARSER_S_ERR;
            parser->err_msg = "Multiple definitions of symbol";
            return;
        }
    }
    expr.num_symbols = parser->num_symbols;

    if (program_execute(p, &expr))
    {
        parser->state = PARSER_S_ERR;
        parser->err_msg = "Execution exception";
    }
    else
        parser->state = PARSER_S_INIT;
}

static void parser_assign(parser_t *parser)
{
    program_t *p = &parser->program;

    if (p->num_assign_exprs >= PARSER_MAX_ASSIGN_EXPRS)
    {
        parser->state = PARSER_S_ERR;
        parser->err_msg = "Exceeded maximum number of assign expressions";
    }
    else
    {
        assign_expr_t *expr = &p->assign_exprs[p->num_assign_exprs];
        memcpy(expr->symbol, parser->symbols[0], parser->symbols_length[0] + 1);
        for (int i = 0; i < parser->num_commands; ++i)
            memcpy(expr->commands[i], parser->commands[i], parser->commands_length[i] + 1);
        expr->num_commands = parser->num_commands;
        p->num_assign_exprs++;
        parser->state = PARSER_S_INIT;
    }
}

void program_print(program_t *p)
{
    for (int as = 0; as < p->num_assign_exprs; ++as)
    {
        printf("Assign: %s = ", p->assign_exprs[as].symbol);
        for (int i = 0; i < p->assign_exprs[as].num_commands; ++i)
        {
            printf("%s", p->assign_exprs[as].commands[i]);
            if (i < p->assign_exprs[as].num_commands - 1)
                printf(" | ");
        }
        printf("\n");
    }
}

void parser_init(parser_t *parser)
{
    parser->state = PARSER_S_INIT;
    parser->comment_active = false;
    parser->program.num_assign_exprs = 0;
}

int parser_feed(parser_t *parser, char c)
{
    if (parser->comment_active)
    {
        if (c == '\n')
            parser->comment_active = false;
        else
            return 0;
    }
    else if (c == '#')
    {
        parser->comment_active = true;
        return 0;
    }

    switch (parser->state)
    {
        case PARSER_S_INIT:
            if (char_is_newline(c) ||
                char_is_whitespace(c));
            else if (char_is_symbol(c))
            {
                parser->num_symbols = 0;
                parser_new_symbol(parser, c);
                parser->state = PARSER_S_SYM1;
            }
            else
            {
                parser->state = PARSER_S_ERR;
                parser->err_msg = "Expected: symbol";
            }
            break;

        case PARSER_S_SYM1:
            if (char_is_symbol(c))
            {
                parser_push_symbol(parser, c);
            }
            else if (char_is_newline(c))
            {
                parser_execute(parser);
            }
            else if (char_is_assign(c))
            {
                parser->state = PARSER_S_ASSIGN_WS;
            }
            else if (char_is_pipe(c))
            {
                parser->state = PARSER_S_PIPE_WS;
            }
            else if (char_is_whitespace(c))
            {
                parser->state = PARSER_S_SYM1_WS;
            }
            else
            {
                parser->state = PARSER_S_ERR;
                parser->err_msg = "Expected: symbol, \\n, =, |, whitespace";
            }
            break;

        case PARSER_S_SYM1_WS:
            if (char_is_whitespace(c));
            else if (char_is_newline(c))
            {
                parser_execute(parser);
            }
            else if (char_is_assign(c))
            {
                parser->state = PARSER_S_ASSIGN_WS;
            }
            else if (char_is_pipe(c))
            {
                parser->state = PARSER_S_PIPE_WS;
            }
            else
            {
                parser->state = PARSER_S_ERR;
                parser->err_msg = "Expected: \\n, =, |, whitespace";
            }
            break;

        case PARSER_S_PIPE_WS:
            if (char_is_symbol(c))
            {
                parser_new_symbol(parser, c);
                parser->state = PARSER_S_PIPE_SYM;
            }
            else if (char_is_whitespace(c));
            else
            {
                parser->state = PARSER_S_ERR;
                parser->err_msg = "Expected: symbol, whitespace";
            }
            break;

        case PARSER_S_PIPE_SYM:
            if (char_is_symbol(c))
            {
                parser_push_symbol(parser, c);
            }
            else if (char_is_whitespace(c))
            {
                parser->state = PARSER_S_PIPE_SYM_WS;
            }
            else if (char_is_newline(c))
            {
                parser_execute(parser);
            }
            else
            {
                parser->state = PARSER_S_ERR;
                parser->err_msg = "Expected: symbol, whitespace, \\n";
            }
            break;

        case PARSER_S_PIPE_SYM_WS:
            if (char_is_whitespace(c));
            else if (char_is_newline(c))
            {
                parser_execute(parser);
            }
            else if (char_is_pipe(c))
            {
                parser->state = PARSER_S_PIPE_WS;
            }
            else
            {
                parser->state = PARSER_S_ERR;
                parser->err_msg = "Expected: |, \\n, whitespace";
            }
            break;

        case PARSER_S_ASSIGN_WS:
            if (char_is_whitespace(c));
            else if (char_is_command(c))
            {
                parser->num_commands = 0;
                parser_new_command(parser, c);
                parser->state = PARSER_S_ASSIGN_CMD;
            }
            else
            {
                parser->state = PARSER_S_ERR;
                parser->err_msg = "Expected: command, whitespace";
            }
            break;

        case PARSER_S_ASSIGN_CMD:
            if (char_is_command(c))
            {
                parser_push_command(parser, c);
            }
            else if (char_is_pipe(c))
            {
                parser->state = PARSER_S_ASSIGN_CMD_PIPE_WS;
            }
            else if (char_is_newline(c))
            {
                parser_assign(parser);
            }
            else
            {
                parser->state = PARSER_S_ERR;
                parser->err_msg = "Expected: command, \\n, |";
            }
            break;

        case PARSER_S_ASSIGN_CMD_PIPE_WS:
            if (char_is_command(c))
            {
                parser_new_command(parser, c);
                parser->state = PARSER_S_ASSIGN_CMD;
            }
            else
            {
                parser->state = PARSER_S_ERR;
                parser->err_msg = "Expected: command";
            }
            break;

        case PARSER_S_ERR:
            break;
    }

    return parser->state == PARSER_S_ERR;
}
