#ifndef JK_10_01_GAME_H
#define JK_10_01_GAME_H

#include <stdbool.h>

typedef enum player_t
{
    PLAYER_X = 1,
    PLAYER_O = -1,
} player_t;

typedef enum winner_t
{
    WINNER_DRAW = 0,
    WINNER_X = 1,
    WINNER_O = -1,
} winner_t;

typedef enum field_t
{
    FIELD_EMPTY = 0,
    FIELD_X = 1,
    FIELD_O = -1,
} field_t;

typedef enum pos_t
{
    POS_NW,
    POS_N,
    POS_NE,
    POS_W,
    POS_C,
    POS_E,
    POS_SW,
    POS_S,
    POS_SE,
} pos_t;

typedef field_t board_t[9];

typedef struct game_t
{
    board_t board;
    player_t next_player;
    winner_t winner;
    bool is_over;
} game_t;

void game_init(game_t *game);

int game_move(game_t *game, pos_t pos);

void board_print(const board_t board);

void game_print(const game_t *game);

#endif
